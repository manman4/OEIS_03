\\ exp(x^5/(120*(1-x)))
a(n) = v=[1,0,0,0,0,1]; if(n<6, v[n+1], 2*(n-1) * a(n-1) - (n-1)*(n-2) * a(n-2) + binomial(n-1,4) * a(n-5) - 4*binomial(n-1,5) * a(n-6));
for(n=0, 15, print1(a(n),", ")) 

