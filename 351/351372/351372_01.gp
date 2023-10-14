N=5000;

for(k=1, N, for(j=1, k, for(i=1, j, if(i*j>k, break); if((i+j)^2+(j+k)^2+(k+i)^2==12*i*j*k, print1(i, ", ", j, ", ", k, ", ")))));

