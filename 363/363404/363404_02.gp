w=exp(2*I*Pi/3);

seq(n) = {
    my(A=1); 
    for(i=1, n, 
        A=exp(sum(k=1, i, sum(m=0, 2, subst(A, x, w^m*x^k))/3*x^k/k)+x*O(x^n))
    ); 
    apply(round, Vec(1/prod(k=0, n\3, (1-x^(3*k+1)+x*O(x^n))^polcoef(A, 3*k))))
};
seq(30)

seq(n) = {
    my(A=1); 
    for(i=1, n, 
        A=exp(sum(k=1, i, sum(m=0, 2, subst(A, x, w^m*x^k))/3*x^k/k)+x*O(x^n))
    ); 
    apply(round, Vec(A * subst(A, x, w*x) * subst(A, x, w^2*x)))
};

\\ A(x) * A(w*x) * A(w^2*x) = A(x^3)
seq(60)

