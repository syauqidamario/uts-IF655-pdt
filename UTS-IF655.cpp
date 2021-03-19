#include <stdio.h>
#include <Windows.h>
#include "mpi.h"

//function ini untuk return value persamaan yang akan diintegralkan
double func_int(double x) {
	double hasil = 3 * x * x + 4 * x + 12;
	return(hasil);
}

//function ini untuk cara menghitung integral dengan metode trapezoid
double trap(double bottom, double top, long n) {
	int iter{};
	double x1, y1;
	double y = 0;
	double hasil = (top - bottom) / n;
	y = 0.5 * hasil * (func_int(top) + func_int(bottom));
	while (iter < n) {
		x1 = bottom + iter * hasil;
		y1 = func_int(x1);
		y += hasil * y1;
		iter++;
	}
	return y;
}

//function ini menampilkan waktu eksekusi dari program
double execTimeDisplay() {
	SYSTEMTIME time;
	GetSystemTime(&time);
	LONG time_ms = (time.wSecond * 1000) + time.wMilliseconds; 
	return(time_ms / 1000.0); 
}

//function main
int main(int argc, char* argv[]) {
	double b_bawah = 10;
	double b_atas = 50;
	long N = 40000000; //jumlah iterasi

	int local_n, my_rank, comm_sz;
	double loc_a, loc_b, hTrap, local_int, total_int;

	MPI_Init(&argc, &argv);

	double time = -execTimeDisplay();

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	hTrap = (b_atas - b_bawah) / N;
	local_n = N / comm_sz;

	loc_a = b_bawah + my_rank * local_n * hTrap;
	loc_b = loc_a + local_n * hTrap;

	double y = trap(loc_a, loc_b, local_n);

	if (my_rank != 0) {
		MPI_Send(&y, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
	else {
		total_int = y;
		for (int i = 1; i < comm_sz; i++) {
			MPI_Recv(&y, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			total_int += y;
		}

		time += execTimeDisplay();
		printf("Nilai Integral dari function = %lf\n", total_int);
		printf("Waktu eksekusinya: %f\n", time);
	}

	MPI_Finalize();

	return 0;
}