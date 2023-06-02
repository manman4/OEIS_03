seq(n) = {
    my(A=1); 
    for(i=1, n, 
        A=exp(sum(k=1, i, subst(A, x, 4*x^k)*x^k/k)+x*O(x^n))
    ); 
    Vec(1/prod(k=0, n, (1-x^(k+1)+x*O(x^n))^(4^k * polcoef(A, k))))
};
seq(20)

a(n) = if(n==0, 1, (1/n) * sum(k=1,n, sumdiv(k,d, d * 4^(d-1) * a(d-1) ) * a(n-k)));  
for(n=0, 15, print1(a(n),", "))
