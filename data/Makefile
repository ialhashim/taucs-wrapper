all: clearlogs cholmod taucs output

clearlogs:
	rm -f *_log.txt
	
cholmod:
	../SurfaceMeshDemo_cholmod/smoothme indorelax_012k.off >> cholmod_log.txt
	../SurfaceMeshDemo_cholmod/smoothme indorelax_024k.off >> cholmod_log.txt
	../SurfaceMeshDemo_cholmod/smoothme indorelax_050k.off >> cholmod_log.txt
	../SurfaceMeshDemo_cholmod/smoothme indorelax_100k.off >> cholmod_log.txt
	../SurfaceMeshDemo_cholmod/smoothme indorelax_194k.off >> cholmod_log.txt

taucs:
	../SurfaceMeshDemo_taucs/smoothme indorelax_012k.off >> taucs_log.txt
	../SurfaceMeshDemo_taucs/smoothme indorelax_024k.off >> taucs_log.txt
	../SurfaceMeshDemo_taucs/smoothme indorelax_050k.off >> taucs_log.txt
	../SurfaceMeshDemo_taucs/smoothme indorelax_100k.off >> taucs_log.txt
	../SurfaceMeshDemo_taucs/smoothme indorelax_194k.off >> taucs_log.txt	
	
output: 
	cat *_log.txt