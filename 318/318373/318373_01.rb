def f(ary)
  n = ary.size
  a = (((ary.join.reverse.to_i) ** 3 + 1) % (10 ** n)).to_s.split('').reverse.map(&:to_i)
  a += [0] * (n - a.size)
  a
end

def A(n)
  ary = [2]
  while ary.size < n
    (0..9).each{|i|
      ary0 = (ary.clone + [i])
      4.times{
        ary0 = f(ary0)
      }
      if (ary.clone + [i]) == ary0
        ary = ary0
        break
      end
    }
  end
  ary
end

n = 1010
ary = A(n + 1)
(0..1000).each{|i|
  j = ary[i]
  print i
  print ' '
  puts j
}
