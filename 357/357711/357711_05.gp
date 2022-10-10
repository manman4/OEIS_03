M=21;

a(n) = my(v=[1, 0, 4]); if(n<3, v[n+1], (n+1)!/2);
for(n=0, M, print1(a(n), ", "));