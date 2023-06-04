seq(n) = my(B=1); for(i=1, n, B=exp(sum(k=1, i, subst(B, x, -x^k)^2*x^k/k)+x*O(x^n))); Vec(B^2);                                             
seq(20)    

seq(n) = {
    my(A=1); 
    for(i=1, n, 
        A=exp(2 * sum(k=1, i, subst(A, x, -x^k) * x^k/k)+x*O(x^n))
    ); 
    Vec(1/prod(k=0, n, (1-x^(k+1)+x*O(x^n))^(2 * (-1)^k * polcoef(A, k))))
};
seq(20)

a(n) = if(n==0, 1, (2/n) * sum(k=1, n, sumdiv(k, d, d * (-1)^(d-1) * a(d-1) ) * a(n-k)));
for(n=0, 15, print1(a(n),", "))
