# ˜aˆöq‚ÍminˆÈãmaxˆÈ‰º
def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end

def A(k, n)
  s_ary = []
  (1..n).each{|m|
    km = k * m
    cnt = 0
    partition(km, 1, km).each{|ary|
      if (ary.size) == m
        cnt += 1
      end
    }
    s_ary << cnt
p cnt
  }
  s_ary
end
p A(2, 10)
p A(3, 10)
p A(4, 10)
p A(5, 10)
