\\ a(n): number of 132-avoiding permutations p such that p^3 is the identity.
\\ Two equivalent PARI/GP formulas are provided:
\\   a(n)     : direct formula with binomial((p/2)+j, j)*4^j,
\\   a_int(n) : integer-only version using 2^j/j! * prod_{k=1..j} (p+2*k).

\\ a(n) = 1 + 2 * Sum_{i=0..floor(n/3)-1} (n-3*i-2)/(i+1) * Sum_{j=0..i} 4^j * binomial((n-3*i-2)/2+j,j) * binomial(2*i-j,i).
a(n) = 1 + sum(i=1, n\3, 2*(n-3*i+1)/i * sum(j=0, i-1, 4^j*binomial((n-3*i+1)/2+j, j)*binomial(2*i-j-2, i-j-1)));

a_int(n) = 1+sum(i=1,n\3,2*(n-3*i+1)/i*sum(j=0,i-1,2^j/j!*prod(k=1,j,n-3*i+1+2*k)*binomial(2*i-j-2,i-j-1)));

\\ Coefficient-extraction form; equivalent but a little less explicit.
a_cf(n)=
{
  my(x='x);
  1+sum(m=1,n\3,2*(n-3*m+1)/m*polcoeff((1-x)^(-m)*(1-4*x+O(x^m))^(-(n-3*m+3)/2),m-1));
};

N=50;
for(n=0, N, print([n,a(n),a_int(n),a_cf(n)]));
\\ for(n=0, N, write("b370686_1.txt", n, " ", a(n)));

A = sum(k=0, N, a(k)*x^k) + x*O(x^N);
\\ G.f. A(x) satisfies (x^4+16*x^3+2*x^2-3)*A(x)^4 + (4*x^3+4*x)*A(x)^3 + (6*x^2+2)*A(x)^2 + 4*x*A(x) + 1 = 0.
(x^4+16*x^3+2*x^2-3)*A^4 + (4*x^3+4*x)*A^3 + (6*x^2+2)*A^2 + 4*x*A + 1
