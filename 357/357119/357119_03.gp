M=11;

Pochhammer(x, n) = prod(k=0, n-1, x+k);
T(n, k) = if(k==0, 0^n, my(w=exp(2*Pi*I/k)); round(sum(j=0, k-1, Pochhammer(w^j, n)))/k);
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));