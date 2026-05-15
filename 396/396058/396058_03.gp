M=80;

\\ G_{k,m}(q) = Sum_{n>=0} b(n,k,m) * q^n = q^(k*(k+1)/2) * Sum_{i=1..min(k,m)} q^((k-i+1)*(m-i)) * q_binomial(m-1,i-1) / Product_{j=1..k-i} (1-q^j).
q_binomial(n,r) = if(r < 0 || r > n, 0, prod(i=1, r, 1 - q^(n-i+1)) / prod(i=1, r, 1 - q^i));
G(k, m) = q^(k*(k+1)/2) * sum(i=1, min(k, m), q^((k-i+1)*(m-i)) * q_binomial(m-1,i-1) / prod(j=1, k-i, 1 - q^j));
Vec(G(5, 3) + q*O(q^M))

q_binomial(3-1,1-1)
q_binomial(3-1,2-1)
q_binomial(3-1,3-1)

\\ a(n) = [q^n] G(5,3)
a(n) = polcoef(G(5, 3) + q*O(q^M), n);

\\ G.f.: Sum_{j=1..5} (-1)^(j-1) * q^(3*j) * Product_{k=1..5-j} q^k/(1-q^k).
c(n) = polcoef(sum(j=1, 5, (-1)^(j-1) * q^(3*j) * prod(k=1, 5-j, q^k/(1-q^k))) + q*O(q^M), n);
for(n=0, 50, print1(a(n) - c(n), ", "));

\\ Let b(n,k,m) be the number of partitions of n into k distinct parts containing the part m.
b(n, k, m) = polcoef(G(k, m) + q*O(q^M), n);

\\ d(n,k,m) = [x^n] G(k,m) = [x^n] q^(k*(k+1)/2) * Sum_{i=1..min(k,m)} q^((k-i+1)*(m-i)) * q_binomial(m-1,i-1) / Product_{j=1..k-i} (1-q^j).
d(n, k, m) = polcoef(G(k, m) + q*O(q^M), n);
for(k=0, 10, for(m=0, 10, for(n=0, 50, if(b(n, k, m) != d(n, k, m), print1("Error: ", n, " ", k, " ", m, "\n")))));
