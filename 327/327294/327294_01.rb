def A(n)
  a = [3, 6]
  b = ([[3]] + (1..n - 1).map{|i| [a[i % 2]] * (2 ** (i - 1))}).reverse.join.to_i
  (b + 1) * (10 ** (2 ** (n - 1)) - b)
end
def A327294(n)
  (1..n).map{|i| A(i)}
end
p A327294(7)
