def A(n)
  return 0 if n < 2
  (1..n).inject(:*) - 1
end

ary = [1, 1]
a = [1]
s_ary = [[1]]
(1..9).each{|i|
  b = a * (i + 1)
  c = ary[0..A(i)] + s_ary[1..-1].flatten
  (0..c.size - 1).each{|j| b[j] += c[j]}
  a = b
  s_ary.unshift(a)
  ary += a * (i + 1)
}
b = (1..ary.max).map{|i| ary.index(i)}
(1..71).each{|i|
  print i
  print ' '
  puts b[i - 1]
}
