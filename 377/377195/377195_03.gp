a(n) = v=[1,10,90]; if(n<3, v[n+1], (2*(2*n+3)*a(n-1) + 8*(n+3)*a(n-2) + 2*(2*n+9)*a(n-3))/n );
for(n=0, 20, print1(a(n),", "))   