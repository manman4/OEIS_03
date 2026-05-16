M=80;

\\ G_{k,m}(q) = Sum_{n>=0} b(n,k,m) * q^n = q^(k*(k+1)/2) * Sum_{i=1..min(k,m)} q^((k-i+1)*(m-i)) * q_binomial(m-1,i-1) / Product_{j=1..k-i} (1-q^j).
q_binomial(n,r) = if(r < 0 || r > n, 0, prod(i=1, r, 1 - q^(n-i+1)) / prod(i=1, r, 1 - q^i));
G(k, m) = q^(k*(k+1)/2) * sum(i=1, min(k, m), q^((k-i+1)*(m-i)) * q_binomial(m-1,i-1) / prod(j=1, k-i, 1 - q^j));

\\ a(n) = [q^n] G(5,3)
a(n) = polcoef(G(5, 3) + q*O(q^M), n);

\\ G.f.: Sum_{j=1..5} (-1)^(j-1) * q^(3*j) * Product_{k=1..5-j} q^k/(1-q^k).
c(n) = polcoef(sum(j=1, 5, (-1)^(j-1) * q^(3*j) * prod(k=1, 5-j, q^k/(1-q^k))) + q*O(q^M), n);
for(n=0, 50, print1(a(n) - c(n), ", "));


\\ G.f.: Sum_{j=1..5} (-1)^(j-1) * q^(3*j) * Product_{k=1..5-j} q^k/(1-q^k).
my(N=70, q='q+O('q^N)); concat([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], Vec(sum(j=1, 5, (-1)^(j-1)*q^(3*j)*prod(k=1, 5-j, q^k/(1-q^k)))))

\\ G.f.: G_{5,3}(q) = q^15 * (1/((1-q)*(1-q^2)) + q^4*(1+q)/((1-q)*(1-q^2)*(1-q^3)) + q^10/((1-q)*(1-q^2)*(1-q^3)*(1-q^4))).
my(N=70, q='q+O('q^N)); Vec(q^15 * (1/((1-q)*(1-q^2)) + q^4*(1+q)/((1-q)*(1-q^2)*(1-q^3)) + q^10/((1-q)*(1-q^2)*(1-q^3)*(1-q^4))) - sum(j=1, 5, (-1)^(j-1)*q^(3*j)*prod(k=1, 5-j, q^k/(1-q^k))))


print("一般");

\\ Let b(n,k,m) be the number of partitions of n into k distinct parts containing the part m.
b(n, k, m) = polcoef(G(k, m) + q*O(q^M), n);

\\ d(n,k,m) = [x^n] G(k,m) = [x^n] Sum_{i=1..k} (-1)^(i-1) * q^(m*i) * Product_{j=1..k-i} q^j/(1-q^j).
d(n, k, m) = polcoef(sum(i=1, k, (-1)^(i-1) * q^(m*i) * prod(j=1, k-i, q^j/(1-q^j))) + q*O(q^M), n);
for(k=0, 10, for(m=1, 10, for(n=0, 50, if(b(n, k, m) != d(n, k, m), print1("Error: ", n, " ", k, " ", m, "\n")))));
