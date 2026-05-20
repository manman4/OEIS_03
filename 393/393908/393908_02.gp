N=55;
M=50;

\\ G.f.: (B(x)/x)^(1/3), where B(x) is the g.f. of A396209.
B(v) = v / (1 - v)^3;
A = (B(B(B(x + x*O(x^N))))/x)^(1/3);

\\ for(n=0, M, write("b393908.txt", n, " ", polcoef(A, n)));
Vec(A)

\\ G.f.: ((1-x) * ((1-x)^3 - x))^2 / (((1-x)^3 - x)^3 - x*(1-x)^6).
Vec( A - ((1-x) * ((1-x)^3 - x))^2 / (((1-x)^3 - x)^3 - x*(1-x)^6) )