default(parisize, 120000000)

seq(n) = {
    my(A=1); 
    for(i=1, n, 
        A=exp(sum(k=1, i, subst(A, x, x^(3*k))*x^k/k)+x*O(x^n))
    ); 
    Vec(1/prod(k=0, n, (1-x^(3*k+1)+x*O(x^n))^polcoef(A, k))) 
};
seq(50)


w = exp(2*Pi*I/3);
seq(n) = {
    my(A=1); 
    for(i=1, n, 
        A=exp(sum(k=1, i, subst(A, x, x^(3*k))*x^k/k)+x*O(x^n))
    ); 
    apply(round, Vec(A * subst(A, x, w*x) * subst(A, x, w^2*x)))
};
\\ A(x^3) = A(x) * A(w*x) * A(w^2*x)
seq(80)


