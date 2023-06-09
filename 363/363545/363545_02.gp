seq(n) = {
    my(A=1); 
    for(i=1, n, 
        A=exp(sum(k=1, i, (2^k + subst(A, x, x^k)) * x^k/k)+x*O(x^n))
    ); 
    Vec((1-2*x+x*O(x^n)) * A)
};
seq(20)

b(n) = if(n==0, 1, (1/n) * sum(k=1, n, (2^k + sumdiv(k, d, d*b(d-1))) * b(n-k)));
a(n) = if(n==0, 1, b(n) - 2*b(n-1));
for(n=0, 15, print1(a(n),", "))
