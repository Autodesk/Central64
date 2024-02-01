#pragma once
#ifndef CENTRAL64_GRID2D
#define CENTRAL64_GRID2D

#include <central64/grid/Array2D.hpp>
#include <central64/grid/Connections.hpp>

namespace central64 {

/// An enumeration of grid cell alignment options.
enum class CellAlignment {
    Center,  ///< Path vertices are placed on the centers of grid cells.
    Corner   ///< Path vertices are placed on the corners of grid cells.
};

/// A 2D grid-based environment representation for `L`-neighbor path planning and line-of-sight checking.
template <int L>
class Grid2D
{
public:
    /// Create a grid from a set of traverable and obstructed cells (`inputCells`) and the specified cell `alignment` option.
    /// If `bool(inputCells[y][x])`, then the cell at coordinates [x, y] is obstructed; otherwise it is traversable.
    /// It is assumed that `inputCells` has the same number of cells in each row `inputCells[y]`.
    Grid2D(const std::vector<std::vector<bool>>& inputCells, 
           CellAlignment alignment = CellAlignment::Center);

    Offset2D Dims() const { return gridGraph_.Dims(); }     ///< Get the number of vertices in each dimension.
    CellAlignment Alignment() const { return alignment_; }  ///< Get the cell alignment.

    const Array2D<bool>& CenterCells() { return centerCells_; }        ///< Obtain a const reference to the array of center-aligned cells.
    const Array2D<bool>& CornerCells() { return cornerCells_; }        ///< Obtain a const reference to the array of corner-aligned cells.
    const Array2D<Connections<L>>& GridGraph() { return gridGraph_; }  ///< Obtain a const reference to the array of sets of connections.

    bool CenterCell(Offset2D coords) const { return centerCells_[coords]; }  ///< Check whether the center-aligned cell at coordinates `coords` is obstructed.
    bool CornerCell(Offset2D coords) const { return cornerCells_[coords]; }  ///< Check whether the corner-aligned cell at coordinates `coords` is obstructed.

    Connections<L> Neighbors(Offset2D coords) const { return gridGraph_[coords]; }  ///< Get the set of connections for the vertex at coordinates `coords`.

    bool Contains(Offset2D coords) const { return gridGraph_.Contains(coords); }  ///< Check whether vertex coordinates `coords` are within the grid.
    bool LineOfSight(Offset2D coordsA, Offset2D coordsB) const;                   ///< Computer whether there is an unobstructed line-of-sight connection between coordinates `coordsA` and `coordsB`.

    Connections<L> CanonicalSuccessors(Offset2D coords, const Move<L>& parentMove) const;  ///< Compute the set of canonical successors at coordinates `coords` if the parent vertex is in the direction `parentMove`.
    Connections<L> ForcedSuccessors(Offset2D coords, const Move<L>& parentMove) const;     ///< Compute the set of forced successors at coordinates `coords` if the parent vertex is in the direction `parentMove`.

private:
    CellAlignment alignment_;
    Array2D<bool> centerCells_;
    Array2D<bool> cornerCells_;
    Array2D<Connections<L>> gridGraph_;
};

/// Obtain a string representation of the specified `grid`.
template <int L>
std::string ToString(const Grid2D<L>& grid);

/// Obtain a string representation of the specified `grid` along with a path represented as a vector of 2D offsets (`pathVertices`).
template <int L>
std::string ToString(const Grid2D<L>& grid, const std::vector<Offset2D>& pathVertices);

template <int L>
Grid2D<L>::Grid2D(const std::vector<std::vector<bool>>& inputCells, 
                  CellAlignment alignment)
    : alignment_{ alignment }
    , centerCells_{}
    , cornerCells_{}
    , gridGraph_{}
{
    // Derive and store the dimensions and both center- and corner-aligned cells.
    int nx = 0;
    int ny = 0;
    if (alignment == CellAlignment::Center) {
        // Store the dimensions of the centered-aligned grid.
        assert(!inputCells.empty());
        assert(!inputCells[0].empty());
        nx = int(inputCells[0].size());
        ny = int(inputCells.size());
        
        // Store the centered-aligned grid cells.
        centerCells_ = Array2D<bool>{ {nx, ny}, false };
        for (int y = 0; y < ny; ++y) {
            assert(int(inputCells[y].size()) == nx);
            for (int x = 0; x < nx; ++x) {
                centerCells_[{x, y}] = inputCells[y][x];
            }
        }

        // Derive the corner-aligned grid cells.
        // Each corner-aligned cell is obstructed if any of the
        // overlappning center-aligned cells are obstructed.
        cornerCells_ = Array2D<bool>{ {nx - 1, ny - 1}, false };
        for (int y = 0; y < ny - 1; ++y) {
            for (int x = 0; x < nx - 1; ++x) {
                cornerCells_[{x, y}] = inputCells[y][x] || inputCells[y][x + 1] ||
                                       inputCells[y + 1][x] || inputCells[y + 1][x + 1];
            }
        }
    }
    else if (alignment == CellAlignment::Corner) {
        // Derive the dimensions of the corner-aligned grid.
        // Because vertices are placed on cell corners, the number of 
        // vertices in each dimension is the number of grid cells plus 1.
        nx = inputCells.empty() ? 1 : int(inputCells[0].size()) + 1;
        ny = int(inputCells.size()) + 1;

        // Store the corner-aligned grid cells and derive the center-aligned grid cells.
        // Each center-aligned cell is obstructed if all of the
        // overlappning corner-aligned cells are obstructed.
        centerCells_ = Array2D<bool>{ {nx, ny}, true };
        cornerCells_ = Array2D<bool>{ {nx - 1, ny - 1}, false };
        for (int y = 0; y < ny - 1; ++y) {
            assert(int(inputCells[y].size()) == nx - 1);
            for (int x = 0; x < nx - 1; ++x) {
                cornerCells_[{x, y}] = inputCells[y][x];
                centerCells_[{x, y}] = centerCells_[{x, y}] && inputCells[y][x];
                centerCells_[{x + 1, y}] = centerCells_[{x + 1, y}] && inputCells[y][x];
                centerCells_[{x, y + 1}] = centerCells_[{x, y + 1}] && inputCells[y][x];
                centerCells_[{x + 1, y + 1}] = centerCells_[{x + 1, y + 1}] && inputCells[y][x];
            }
        }
    }

    // Use line-of-sight checks to populate a graph of connections among neighboring grid vertices.
    gridGraph_ = Array2D<Connections<L>>{ {nx, ny} };
    for (int y = 0; y < ny; ++y) {
        for (int x = 0; x < nx; ++x) {
            const Offset2D coords{ x, y };
            Connections<L>& neighbors = gridGraph_[coords];
            for (const Move<L>& move : Neighborhood<L>::Moves()) {
                const Offset2D neighborCoords = coords + move.Offset();
                if (Contains(neighborCoords)) {
                    if (LineOfSight(coords, neighborCoords)) {
                        neighbors.Connect(move);
                    }
                }
            }
        }
    }
}

template <int L>
bool Grid2D<L>::LineOfSight(Offset2D coordsA, Offset2D coordsB) const
{
    assert(Contains(coordsA));
    assert(Contains(coordsB));

    bool lineOfSight = true;
    if (coordsA.Y() == coordsB.Y()) {
        // The Y coordinates are equal. Use the original set of grid cells 
        // (center- or corner-aligned, whichever was supplied as an input)
        // to check whether the sightline is clear.
        const int x0 = std::min(coordsA.X(), coordsB.X());
        const int x1 = std::max(coordsA.X(), coordsB.X());
        const int y = coordsA.Y();
        if (alignment_ == CellAlignment::Center) {
            for (int x = x0; lineOfSight && x <= x1; ++x) {
                lineOfSight = !centerCells_[{x, y}];
            }
        }
        else {
            for (int x = x0; lineOfSight && x < x1; ++x) {
                const bool lowerCell = (y == 0)              ? true : cornerCells_[{x, y - 1}];
                const bool upperCell = (y == Dims().Y() - 1) ? true : cornerCells_[{x, y}];
                lineOfSight = !(lowerCell && upperCell);
            }
        }
    }
    else if (coordsA.X() == coordsB.X()) {
        // The X coordinates are equal. Use the original set of grid cells 
        // (center- or corner-aligned, whichever was supplied as an input)
        // to check whether the sightline is clear.
        const int x = coordsA.X();
        const int y0 = std::min(coordsA.Y(), coordsB.Y());
        const int y1 = std::max(coordsA.Y(), coordsB.Y());
        if (alignment_ == CellAlignment::Center) {
            for (int y = y0; lineOfSight && y <= y1; ++y) {
                // The sightline is still clear if the current cell is traversable.
                lineOfSight = !centerCells_[{x, y}];
            }
        }
        else {
            for (int y = y0; lineOfSight && y < y1; ++y) {
                const bool lowerCell = (x == 0)              ? true : cornerCells_[{x - 1, y}];
                const bool upperCell = (x == Dims().X() - 1) ? true : cornerCells_[{x, y}];
                // The sightline is still clear if either surrounding cell is traversable.
                lineOfSight = !(lowerCell && upperCell);
            }
        }
    }
    else {
        // The X and Y coordinates both differ. Use the corner-aligned 
        // grid cells only to check whether the sightline is clear.
        const int dx = ((coordsB.X() - coordsA.X()) < 0) ? -1 : 1;
        const int dy = ((coordsB.Y() - coordsA.Y()) < 0) ? -1 : 1;
        const int64_t deltaX = std::abs(int64_t(coordsB.X() - coordsA.X()));
        const int64_t deltaY = std::abs(int64_t(coordsB.Y() - coordsA.Y()));
        const int64_t totalQuanta = deltaX*deltaY;

        // Start at coordinates `coordsA` and advance along the sightline to coordinates `coordsB`.
        int64_t currentQuanta = 0;
        int64_t nextQuantaX = deltaY;
        int64_t nextQuantaY = deltaX;
        Offset2D cornerCoords{ coordsA.X() + std::min(dx, 0), coordsA.Y() + std::min(dy, 0) };
        while (lineOfSight && currentQuanta != totalQuanta) {
            if (cornerCells_[cornerCoords]) {
                // The current cell is obstructed, so the sightline is not clear.
                lineOfSight = false;
            }
            else {
                // The current cell is traversable, so advance along the sightline.
                if (nextQuantaX < nextQuantaY) {
                    // The sightline crosses the cell boundary in the X direction.
                    currentQuanta = nextQuantaX;
                    nextQuantaX += deltaY;
                    cornerCoords = { cornerCoords.X() + dx, cornerCoords.Y() };
                }
                else if (nextQuantaY < nextQuantaX) {
                    // The sightline crosses the cell boundary in the Y direction.
                    currentQuanta = nextQuantaY;
                    nextQuantaY += deltaX;
                    cornerCoords = { cornerCoords.X(), cornerCoords.Y() + dy };
                }
                else {
                    // The sightline crosses the cell boundary at the corner.
                    currentQuanta = nextQuantaX;
                    nextQuantaX += deltaY;
                    nextQuantaY += deltaX;
                    cornerCoords = { cornerCoords.X() + dx, cornerCoords.Y() + dy };
                }
            }
        }
    }
    return lineOfSight;
}

template <int L>
Connections<L> Grid2D<L>::CanonicalSuccessors(Offset2D coords, const Move<L>& parentMove) const
{
    Connections<L> successors{};
    const Connections<L> neighbors = gridGraph_[coords];
    const Move<L> forwardMove = -parentMove;

    // Add the forward move as a canonical successor if it is connected.
    if (neighbors.IsConnected(forwardMove)) {
        successors.Connect(forwardMove);
    }

    // Add the two surrounding moves as canonical successors if the parent move is odd and if they are connected.
    if (parentMove.IsOdd()) {
        const Move<L> slightTurnPositive = forwardMove + 1;
        if (neighbors.IsConnected(slightTurnPositive)) {
            successors.Connect(slightTurnPositive);
        }
        const Move<L> slightTurnNegative = forwardMove - 1;;
        if (neighbors.IsConnected(slightTurnNegative)) {
            successors.Connect(slightTurnNegative);
        }
    }
    return successors;
}

template <int L>
Connections<L> Grid2D<L>::ForcedSuccessors(Offset2D coords, const Move<L>& parentMove) const
{
    Connections<L> successors{};
    const Connections<L> neighbors = gridGraph_[coords];
    const Move<L> forwardMove = -parentMove;
    if constexpr (L == 4) {
        // For the 4-neighorhood, add the 90-degree turns as forced successors 
        // if they are connected and if they wrap around an obstructed cells.
        const Move<L> squareTurnPositive = forwardMove + Move<L>::TicksPer90Degrees();
        if (neighbors.IsConnected(squareTurnPositive)) {
            Offset2D cornerCoords = (2*coords + parentMove.Offset() + squareTurnPositive.Offset())/2;
            if (cornerCells_[cornerCoords]) {
                successors.Connect(squareTurnPositive);
            }
        }
        const Move<L> squareTurnNegative = forwardMove - Move<L>::TicksPer90Degrees();
        if (neighbors.IsConnected(squareTurnNegative)) {
            const Offset2D cornerCoords = (2*coords + parentMove.Offset() + squareTurnNegative.Offset())/2;
            if (cornerCells_[cornerCoords]) {
                successors.Connect(squareTurnNegative);
            }
        }
    }
    else {
        // For larger neighorhoods, search the 180 degree arc from the parent move
        // to the forward move, in both directions. If an obstacle is found, add
        // all subsequent connected moves as forced successors.
        bool passingObstaclePositive = false;
        for (Move<L> move = parentMove - 1; move != forwardMove; --move) {
            if (neighbors.IsConnected(move)) {
                if (passingObstaclePositive) {
                    successors.Connect(move);
                }
            }
            else {
                if (move.IsDiagonal()) {
                    passingObstaclePositive = true;
                }
            }
        }
        bool passingObstacleNegative = false;
        for (Move<L> move = parentMove + 1; move != forwardMove; ++move) {
            if (neighbors.IsConnected(move)) {
                if (passingObstacleNegative) {
                    successors.Connect(move);
                }
            }
            else {
                if (move.IsDiagonal()) {
                    passingObstacleNegative = true;
                }
            }
        }
    }
    return successors;
}

template <int L>
inline std::string ToString(const Grid2D<L>& grid)
{
    return ToString(grid, {});
}

template <int L>
inline std::string ToString(const Grid2D<L>& grid, const std::vector<Offset2D>& pathVertices)
{
    std::string gridString{};

    // Represent the path as a 2D array of digits.
    // Each path vertex is assigned the last digit of its index.
    // All grid vertices that are not part of the path are set to -1.
    Array2D<int> path2D{ grid.Dims(), -1 };
    int vertexCount = int(pathVertices.size());
    for (int i = 0; i < vertexCount; ++i) {
        path2D[pathVertices[i]] = i%10;
    }

    // Construct the string representation of the grid and path.
    for (int y = 0; y < grid.Dims().Y(); ++y) {
        // Encode the current row of vertices.
        for (int x = 0; x < grid.Dims().X(); ++x) {
            // Encode the vertex based on the center-aligned grid cell.
            gridString += !grid.CenterCell({ x, y }) ? '+' : ' ';

            // Replace the last character if the vertex is part of the path.
            if (path2D[{x, y}] != -1) {
                std::to_chars(gridString.data() + gridString.size() - 1, gridString.data() + gridString.size(), path2D[{x, y}]);
            }

            // Encode the connection between the current and next vertex in the row.
            if (x < grid.Dims().X() - 1) {
                gridString += grid.LineOfSight({ x, y }, { x + 1, y }) ? "--" : "  ";
            }
        }
        gridString += '\n';

        // Encode the connections and corner grid cells between the current and next row.
        if (y < grid.Dims().Y() - 1) {
            for (int x = 0; x < grid.Dims().X(); ++x) {
                gridString += grid.LineOfSight({ x, y }, { x, y + 1 }) ? '|' : ' ';
                if (x < grid.Dims().X() - 1) {
                    gridString += !grid.CornerCell({ x, y }) ? "><" : "  ";
                }
            }
            gridString += '\n';
        }
    }
    return gridString;
}

}  // namespace

#endif
