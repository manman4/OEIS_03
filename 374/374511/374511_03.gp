a(n) = v=[1,10]; if(n<2, v[n+1], (2*(2*n+3)*a(n-1) + 4*(n+3)*a(n-2))/n );
for(n=0, 22, print1(a(n),", "))   