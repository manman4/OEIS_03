a(n) = v=[1,0,10]; if(n<3, v[n+1], (4*(n+3)*a(n-2) + 2*(2*n+9)*a(n-3))/n );
for(n=0, 20, print1(a(n),", "))   