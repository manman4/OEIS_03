M=21;

a(n, k) = my(A=1); for(i=1, n, A = 1/(1 - x * subst(A, x, x*A) )^2 + x*O(x^n) ); Vec(A^k);
for(k=1, 10, print(k,": ",a(M, k)));

\\ G.f.: B(x)^2, where B(x) is the g.f. of A381029.
print(a(M, 1/2));