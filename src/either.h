#ifndef _EITHER_H_
#define _EITHER_H_

#include <nan.h>
#include <v8.h>

namespace node_webrtc {

template<typename L, typename R> class Either {
  public:
    Either(
        Nan::Maybe<L> left,
        Nan::Maybe<R> right):
      left(left),
      right(right)
    {
      assert(left.IsJust() ^ right.IsJust());
      this->isLeft = left.IsJust();
      this->isRight = right.IsJust();
    }

    static Either<L, R> Left(L left)
    {
      return Either(Nan::Just(left), Nan::Nothing<R>());
    }

    static Either<L, R> Right(R right)
    {
      return Either(Nan::Nothing<L>(), Nan::Just(right));
    }

    L FromLeft() { return this->left.FromJust(); }
    R FromRight() { return this->right.FromJust(); }
    Nan::Maybe<L> GetLeft() { return this->left; }
    Nan::Maybe<R> GetRight() { return this->right; }

  private:
    bool isLeft;
    bool isRight;
    Nan::Maybe<L> left;
    Nan::Maybe<R> right;
};

}

#endif  // _EITHER_H_
