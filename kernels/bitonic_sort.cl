__kernel void bitonicSort(__global int* data, int N, int stage, int step) {
    int id = get_global_id(0);
    
    int pairIdx = id ^ step; // Определяем пару для сравнения

    if (pairIdx > id) {
        bool ascending = ((id & stage) == 0);
        if ((data[id] > data[pairIdx]) == ascending) {
            int temp = data[id];
            data[id] = data[pairIdx];
            data[pairIdx] = temp;
        }
    }
}