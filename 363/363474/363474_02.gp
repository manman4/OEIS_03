seq(n) = {
    my(A=1); 
    for(i=1, n, 
        A=exp(2 * sum(k=1, i, (-1)^(k+1) * subst(A, x, -x^k) * x^k/k)+x*O(x^n))
    ); 
    Vec(prod(k=0, n, (1+x^(k+1)+x*O(x^n))^(2 * (-1)^k * polcoef(A, k))))
};
seq(25)

a(n) = if(n==0, 1, (2/n) * sum(k=1, n, sumdiv(k, d, d * (-1)^(d+k/d) * a(d-1) ) * a(n-k)));
for(n=0, 15, print1(a(n),", "))
