\\ exp((x + x^2/2)/(1-x))
a(n) = v=[1,1,4]; if(n<3, v[n+1], (2*n-1) * a(n-1) - (n-1)*(n-3) * a(n-2) - binomial(n-1,2) * a(n-3));
for(n=0, 15, print1(a(n),", ")) 

