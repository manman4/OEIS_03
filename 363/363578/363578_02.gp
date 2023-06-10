seq(n) = {
    my(A=1); 
    for(i=1, n, 
        A=exp(sum(k=1, i, ((-2)^k + subst(A, x, x^k)) * x^k/k)+x*O(x^n))
    ); 
    Vec(1/(1+2*x+x*O(x^n)) * 1/prod(k=0, n, (1-x^(k+1)+x*O(x^n))^polcoef(A, k)))
};
seq(30)

a(n) = if(n==0, 1, (1/n) * sum(k=1, n, ((-2)^k + sumdiv(k, d, d * a(d-1))) * a(n-k)));
for(n=0, 15, print1(a(n),", "))

