M=22;

Pochhammer(x, n) = prod(k=0, n-1, x+k);
a(n) = my(w=(-1+sqrt(3)*I)/2); round(Pochhammer(1, n)+w^2*Pochhammer(w, n)+w*Pochhammer(w^2, n))/3;
for(n=0, M, print1(a(n), ", "));