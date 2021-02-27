
def A(n)
p ''
  f_ary = [[0]]
  (1..n).each{|i|
    b_ary = []
    f_ary.each{|a|
      b_ary << a + [i]
      b_ary << a + [-i]
    }
    f_ary = b_ary
  }
  cnt = 0
  f_ary.each{|i|
    if i.inject(:+) % n == n - 1
p [i[1..-1], i.inject(:+)]
      cnt += 1 
    end
  }
  cnt
end

p (1..7).map{|i| A(i)}
