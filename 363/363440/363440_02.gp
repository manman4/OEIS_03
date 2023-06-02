seq(n) = {
    my(A=1); 
    for(i=1, n, 
        A=exp(sum(k=1, i, subst(A, x, x^k) * (4*x)^k/k)+x*O(x^n))
    ); 
    Vec(1/prod(k=0, n, (1-4*x^(k+1)+x*O(x^n))^polcoef(A, k)))
};
seq(20)

a(n) = if(n==0, 1, (1/n) * sum(k=1, n, sumdiv(k, d, d * 4^(k/d) * a(d-1) ) * a(n-k)));
for(n=0, 15, print1(a(n),", "))
