def f(ary)
  f_ary = []
  while !(ary.all?{|i| i == 0})
    a = []
    s = 0
    ary.each{|i|
      if i == 1
        a << 0
        s += 1
      elsif i > 0
        a << i - 2
        s += 2
      end
    }
    ary = a
    f_ary << s
  end
  f_ary
end
def g(n, m = 0, a = [])
  if n <= m
    @ary << a.clone if n == m && (a.reverse == f(a))
  else
    s = 1
    s = [1, a[-1] + a[-1] % 2].max if m > 0
    (s..n).each{|x|
      a.push x
      g(n, m + x, a)
      a.pop
    }
  end
end
def A316384(n)
  ary = []
  (0..n).each{|i|
    @ary = []
    g(i)
    ary << @ary.size
  }
  ary
end
p A316384(70)
