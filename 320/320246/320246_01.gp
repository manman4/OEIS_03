M=1000;

a(n) = local(f=1+2*sum(k=1, n, issquare(k)*x^k)+x*O(x^n)); Vec(prod(m=1, 12, subst(f, x, x^m+x*O(x^n))));
a(M)
