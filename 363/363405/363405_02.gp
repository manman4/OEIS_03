w=I;

seq(n) = {
    my(A=1); 
    for(i=1, n, 
        A=exp(sum(k=1, i, sum(m=0, 3, subst(A, x, w^m*x^k))/4*x^k/k)+x*O(x^n))
    ); 
    Vec(1/prod(k=0, n\4, (1-x^(4*k+1)+x*O(x^n))^polcoef(A, 4*k)))
};
seq(30)

seq(n) = {
    my(A=1); 
    for(i=1, n, 
        A=exp(sum(k=1, i, sum(m=0, 3, subst(A, x, w^m*x^k))/4*x^k/k)+x*O(x^n))
    ); 
    Vec(A * subst(A, x, w*x) * subst(A, x, w^2*x) * subst(A, x, w^3*x))
};

\\ A(x) * A(w*x) * A(w^2*x) * A(w^3*x) = A(x^4)
seq(80)

