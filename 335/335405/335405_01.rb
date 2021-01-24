def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end
def g(ary, n)
  sum = ary.inject(:+)
  a = [n - sum] + ary.group_by(&:to_i).map{|i| i.last.size}
  f(a.inject(:+)) / a.inject(1){|s, i| s * f(i)}
end
def A(n)
  return 0 if n == 0
  s = 1
  f_ary = (2..n / 2).select{|i| n % i == 0}.map{|i| [i]}
  while f_ary.size > 0
    b_ary = []
    f_ary.each{|i|
      k = n / i.inject(:*)
      (i[-1]..k).each{|j|
        if j < k
          b_ary << i + [j] if k % j == 0
        else
          s += g(i + [j], n)
        end
      }
    }
    f_ary = b_ary
  end
  s
end
# def A335405(n)
#   (0..n).map{|i| A(i)}
# end

# p A335405(53)

n = 10000
(0..n).each{|i|
  print i
  print ' '
  puts A(i)
}