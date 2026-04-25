# a(n): [2n] の derangement（不動点なし）で descents が n 個の個数
# 定義どおりにバックトラックで全探索します（小さい n 向け）。

def count_derangements_with_n_descents(n)
  m = 2 * n
  return 1 if n == 0

  used = Array.new(m + 1, false) # 1..m
  perm = Array.new(m, 0)
  count = 0

  dfs = uninitialized Proc(Int32, Int32, Nil)
  dfs = ->(pos : Int32, desc : Int32) {
    # pos: 0-based index in perm
    if pos == m
      count += 1 if desc == n
      return
    end

    1.upto(m){|x|
      next if used[x]
      next if x == pos + 1 # fixed point禁止（1-based位置）

      new_desc = desc
      new_desc += 1 if pos > 0 && perm[pos - 1] > x
      next if new_desc > n

      used[x] = true
      perm[pos] = x
      dfs.call(pos + 1, new_desc)
      used[x] = false
    }
  }

  dfs.call(0, 0)
  count
end

p (0..6).map{|i| count_derangements_with_n_descents(i)}
