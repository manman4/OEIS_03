def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end
def mul(f_ary, b_ary)
  s1, s2 = f_ary.size, b_ary.size
  ary = Array.new(s1 + s2 - 1, 0)
  (0..s1 - 1).each{|i|
    (0..s2 - 1).each{|j|
      ary[i + j] += f_ary[i] * b_ary[j]
    }
  }
  ary
end
def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end
def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end
def A(a)
  ary = [1]
  a.each{|i|
    ary = mul(ary, [0] + (1..i).map{|j| (-1) ** (i - j) * ncr(i - 1, i - j) / f(j).to_r})
  }
  (0..ary.size - 1).inject(0){|s, i| s + f(i) * ary[i]}.to_i
end
def A321686(n)
  a = Array.new(n + 1, 0)
  partition(n, 1, n).each{|ary|
    a[ary.size] += A(ary)
  }
  a[1..-1]
end

ary = []
(1..30).each{|i| ary << A321686(i)}
ary.flatten!
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
