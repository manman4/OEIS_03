M=20;

a(n) = my(v=[1,2]); if(n<2, v[n+1], (2*(7*n-6)*a(n-1) - 45*(n-2)*a(n-2))/n);  
for(n=0, M, print1(a(n), ", "));  
