\\ exp(x^3/(6*(1-x)))
a(n) = v=[1,0,0,1]; if(n<4, v[n+1], 2*(n-1) * a(n-1) - (n-1)*(n-2) * a(n-2) + binomial(n-1,2) * a(n-3) - 2*binomial(n-1,3) * a(n-4));
for(n=0, 15, print1(a(n),", ")) 

