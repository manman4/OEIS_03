M=20;

a(n) = my(v=[1,2,6,20,72,264]); if(n<6, v[n+1], (2*(2*n-1)*a(n-1) + 2*(n-3)*a(n-3) - 2*(2*n-10)*a(n-4) - (n-6)*a(n-6))/n);  
for(n=0, M, print1(a(n), ", "));  
