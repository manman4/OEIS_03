def A(n)
  a, b = (0..n).map {|x| x ** 2 + 1}, [0] * (n + 1)
    1.upto(n) do |x|
p x
      next if (q = a[x]) == 1
      x.step(n, q) do |y|
        while a[y] % q == 0
	  a[y] /= q
        end
        b[y] += 1
      end
    end
  b
end

i = 7
ary = A(10 ** i)
p (1..10 ** i).select{|j| ary[j] == 1}.inject(0){|s, j| s += j * j + 1}
