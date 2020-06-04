ZETA_K=3
MIN_ITV_LEN=4
RES_SEG_LEN=256
CTA_SIZE=256
NVCC=/usr/local/cuda/bin/nvcc
bfs_gcgt:
	${NVCC} -I ./cub -O3 -std=c++11 -gencode arch=compute_61,code=sm_61  -odir "." -M -o "main.d" "main.cu"
	${NVCC} -DZETA_K=${ZETA_K} -DMIN_ITV_LEN=${MIN_ITV_LEN} -DRESIDUAL_SEGMENT_LEN=${RES_SEG_LEN} -DCTA_SIZE=${CTA_SIZE} -I ./cub -O3 -std=c++11 --compile --relocatable-device-code=false -gencode arch=compute_61,code=compute_61 -gencode arch=compute_61,code=sm_61  -x cu -o  "main.o" "main.cu"
	${NVCC} --cudart static --relocatable-device-code=false -gencode arch=compute_61,code=compute_61 -gencode arch=compute_61,code=sm_61 -link -o  "bfs_gcgt"  ./main.o
clean:
	rm ./main.o ./main.d bfs_gcgt
