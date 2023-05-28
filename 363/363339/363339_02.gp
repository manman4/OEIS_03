default(parisize, 120000000)

seq(n) = {
    my(A=1); 
    for(i=1, n, 
        A=exp(sum(k=1, i, (-1)^(k+1) * subst(A, x, x^(4*k))*x^k/k)+x*O(x^n))
    ); 
    Vec(prod(k=0, n, (1+x^(4*k+1)+x*O(x^n))^polcoef(A, k))) 
};
seq(100)


w = I;
seq(n) = {
    my(A=1); 
    for(i=1, n, 
        A=exp(sum(k=1, i, (-1)^(k+1) * subst(A, x, x^(4*k))*x^k/k)+x*O(x^n))
    ); 
    Vec(A * subst(A, x, w*x) * subst(A, x, w^2*x) * subst(A, x, w^3*x))
};
\\ A(-x^4) = A(x) * A(w*x) * A(w^2*x) * A(w^3*x)
seq(130)


