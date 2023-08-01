#include <central64/grid/Neighborhood.hpp>

using namespace central64;

template <int L>
void PrintNeighborhood()
{
    printf("%d-Neighborhood:\n", L);
    printf("  Radius = %d\n", Neighborhood<L>::Radius());
    for (const Move<L>& move : Neighborhood<L>::Moves()) {
        printf("  Move %02d: Offset = [%2d, %2d]; Cost = %1.6f;%s%s\n", 
               move.Index(),
               move.Offset().X(),
               move.Offset().Y(),
               move.Cost().Distance(),
               move.IsCardinal() ? " (cardinal)" : "",
               move.IsDiagonal() ? " (diagonal)" : "");
    }
    printf("\n");
}

void PrintNeighborhoods()
{
    PrintNeighborhood< 4>();
    PrintNeighborhood< 8>();
    PrintNeighborhood<16>();
    PrintNeighborhood<32>();
    PrintNeighborhood<64>();
}
