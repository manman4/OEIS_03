a007088(n) = fromdigits(binary(n), 10);
a305989(n) = fromdigits(Vecrev(digits(n, 2)), 10);
a(n) = a007088(n)*10^(#binary(n))+a305989(n);

M=20;
for(n=0, M, print1(a(n), ", ")); 