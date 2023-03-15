\\ exp((x + x^2/2 + x^3/6 + x^4/24)/(1-x))
a(n) = v=[1,1,4,20,127]; if(n<5, v[n+1], (2*n-1) * a(n-1) - (n-1)*(n-3) * a(n-2) - binomial(n-1,3) * a(n-4) - 3*binomial(n-1,4) * a(n-5));
for(n=0, 15, print1(a(n),", ")) 

