a007088(n) = fromdigits(binary(n), 10);
a305989(n) = fromdigits(Vecrev(digits(n, 2)), 10);
a(n) = a007088(n-1)*10^(#binary(n-1))+a305989(n-1);

M=20;
for(n=1, M, print1(a(n), ", ")); 