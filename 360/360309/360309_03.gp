M=20;

a(n) = my(v=[1,0,0,2]); if(n<4, v[n+1], (2*(n-1)*a(n-1) - (n-2)*a(n-2) + 2*(2*n-3)*a(n-3) - 2*(2*n-6)*a(n-4))/n);  
for(n=0, M, print1(a(n), ", "));  



