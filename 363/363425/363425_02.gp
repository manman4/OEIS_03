seq(n) = {
    my(A=1); 
    for(i=1, n, 
        A=exp(sum(k=1, i, (-1)^(k+1)*subst(A, x, 2*x^k)*x^k/k)+x*O(x^n))
    ); 
    Vec(prod(k=0, n, (1+x^(k+1)+x*O(x^n))^(2^k * polcoef(A, k))))
};
seq(20)

a(n) = if(n==0, 1, (1/n) * sum(k=1, n, sumdiv(k, d, (-1)^(k/d+1) * d * 2^(d-1) * a(d-1) ) * a(n-k)));
for(n=0, 15, print1(a(n),", "))
