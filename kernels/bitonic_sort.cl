__kernel void bitonic_sort(__global int* data, int N) {
    int idx = get_global_id(0);  // Индекс потока
    int i, j, k;

    for (k = 2; k <= N; k *= 2) {
        for (j = k / 2; j > 0; j /= 2) {
            int ixj = idx ^ j;

            if (ixj > idx) {
                // Определяем направление сортировки
                bool ascending = ((idx & k) == 0);
                if ((data[idx] > data[ixj]) == ascending) {
                    // Обмен значений
                    int tmp = data[idx];
                    data[idx] = data[ixj];
                    data[ixj] = tmp;
                }
            }

            barrier(CLK_GLOBAL_MEM_FENCE);  // Синхронизация потоков
        }
    }
}