/*
This code was developed by Tansel Uras (turas@usc.edu) at USC.
The code is hosted at 'http://idm-lab.org/anyangle'.
If you use this code in your research, please  cite our SoCS paper:

T. Uras and S. Koenig,  2015. An Empirical Comparison of Any-Angle Path-Planning Algorithms. In: Proceedings of the 8th Annual Symposium on Combinatorial
Search. Code available at: http://idm-lab.org/anyangle

Bibtex:
@inproceedings{uras:15,
  author = "T. Uras and S. Koenig",
  title = "An Empirical Comparison of Any-Angle Path-Planning Algorithms",
  booktitle = {Proceedings of the 8th Annual Symposium on Combinatorial Search},
  year = "2015",
  note = "Code available at: http://idm-lab.org/anyangle",
}
*/

#ifndef ANY_ANGLE_HOG
#define ANY_ANGLE_HOG

#include "ThetaStar.h"
#include "BlockAStar.h"
#include "FieldAStar.h"
#include "SubgoalAA.h"
#include "ANYA.h"


//TODO: 'Reference Arguments': all reference parameters should be 'const'. Use pointers for parameters that the function can modify. (not likely)
//TODO: Const (maybe, after everything else is done)

//TODO: Differentiate between xyLoc as corners and grid cells? Not likely...
//TODO: cout vs printf: use cout?

//TODO: cornerId -> CornerId ?

//TODO: split xyLoc into cellLoc and cornerLoc? Maybe linearize cellLoc into mapLoc and keep a copy of the transposed map. This way, LOS checks can be
// written in a simpler way: Iterating over x? use the regular one. Iterating over y? use the transposed map. A single function can handle iteration over
// the line.



class AnyAngleHOG{
public:
    AnyAngleHOG(MapEnvironment* mEnv)
    {
        a_star_euc_ = NULL;
        a_star_oct_ = NULL;
        theta_star_ = NULL;
        lazy_theta_star_ = NULL;
        block_a_star_ = NULL;
        field_a_star_ = NULL;
        anya_ = NULL;
        sub_1_level_a_star_ = NULL;
        sub_2_level_a_star_ = NULL;
        sub_10000_level_a_star_ = NULL;
        sub_1_level_theta_star_ = NULL;
        sub_2_level_theta_star_ = NULL;
        sub_10000_level_theta_star_ = NULL;

        // Comment out from the following
        //a_star_euc_                 = new ThetaStar(mEnv, A_STAR_EUC);
        //a_star_oct_                 = new ThetaStar(mEnv, A_STAR_OCT);
        theta_star_                 = new ThetaStar(mEnv, THETA_STAR);
        //lazy_theta_star_            = new ThetaStar(mEnv, LAZY_THETA_STAR);

        //block_a_star_               = new BlockAStar(mEnv);

        //field_a_star_                = new FieldAStar(mEnv);

        //anya_                        = new ANYA(mEnv);


        //sub_1_level_a_star_         = new SubgoalAA(mEnv, 1, SUBGOAL_A_STAR);
        //sub_2_level_a_star_         = new SubgoalAA(mEnv, 2, SUBGOAL_A_STAR);
        //sub_10000_level_a_star_     = new SubgoalAA(mEnv, 1000, SUBGOAL_A_STAR);

        //sub_1_level_theta_star_     = new SubgoalAA(mEnv, 1, SUBGOAL_THETA_STAR);
        //sub_2_level_theta_star_     = new SubgoalAA(mEnv, 2, SUBGOAL_THETA_STAR);
        sub_10000_level_theta_star_ = new SubgoalAA(mEnv, 1000, SUBGOAL_THETA_STAR);

        if (a_star_euc_ != NULL)                    algorithms_.push_back((AnyAngleAlgorithm*)a_star_euc_);
        if (a_star_oct_ != NULL)                    algorithms_.push_back((AnyAngleAlgorithm*)a_star_oct_);
        if (theta_star_ != NULL)                    algorithms_.push_back((AnyAngleAlgorithm*)theta_star_);
        if (lazy_theta_star_ != NULL)               algorithms_.push_back((AnyAngleAlgorithm*)lazy_theta_star_);
        if (block_a_star_ != NULL)                  algorithms_.push_back((AnyAngleAlgorithm*)block_a_star_);
        if (field_a_star_ != NULL)                  algorithms_.push_back((AnyAngleAlgorithm*)field_a_star_);
        if (anya_ != NULL)                          algorithms_.push_back((AnyAngleAlgorithm*)anya_);
        if (sub_1_level_a_star_ != NULL)            algorithms_.push_back((AnyAngleAlgorithm*)sub_1_level_a_star_);
        if (sub_2_level_a_star_ != NULL)            algorithms_.push_back((AnyAngleAlgorithm*)sub_2_level_a_star_);
        if (sub_10000_level_a_star_ != NULL)        algorithms_.push_back((AnyAngleAlgorithm*)sub_10000_level_a_star_);
        if (sub_1_level_theta_star_ != NULL)        algorithms_.push_back((AnyAngleAlgorithm*)sub_1_level_theta_star_);
        if (sub_2_level_theta_star_ != NULL)        algorithms_.push_back((AnyAngleAlgorithm*)sub_2_level_theta_star_);
        if (sub_10000_level_theta_star_ != NULL)    algorithms_.push_back((AnyAngleAlgorithm*)sub_10000_level_theta_star_);
    }
    ~AnyAngleHOG()
    {
        for (int i = 0; i < algorithms_.size(); i++)
            delete algorithms_[i];
    }

    void FindPaths(xyLoc s, xyLoc g)
    {
        for (int i = 0; i < algorithms_.size(); i++)
            algorithms_[i]-> FindPath(s,g);
    }

    void SetDebugLoc(xyLoc l)
    {
        // TODO
        for (int i = 0; i < algorithms_.size(); i++)
            algorithms_[i]-> SetDebugLoc(l);
    }

    void Visualize(const MapEnvironment *env)
    {
        //for (int i = 0; i < algorithms_.size(); i++)    algorithms_[i]-> ShowSmoothedPath(env);
        if (anya_) {
            anya_->ShowPath(env, 1, 0, 0);
            //anya_->VisualizeAlgorithm(env);
        }

        if (theta_star_) {
            theta_star_->ShowPath(env, 0, 0, 1);
        }

        if (field_a_star_) {
            field_a_star_ -> ShowPath(env, 1, 0, 0);
            field_a_star_ -> ShowSmoothedPath(env, 0, 1, 0);
        }

        if (a_star_oct_) {
            a_star_oct_ -> ShowPath(env, 1, 0, 0);
            a_star_oct_ -> ShowSmoothedPath(env, 0, 0, 1);
        }

        if (sub_1_level_a_star_) {
            sub_1_level_a_star_ -> ShowPath(env, 1,0,0);
            sub_1_level_a_star_ -> VisualizeAlgorithm(env);
        }

        if (sub_10000_level_theta_star_) {
            sub_10000_level_theta_star_ -> ShowPath(env, 1,0,0);
            sub_10000_level_theta_star_ -> VisualizeAlgorithm(env);
        }
    }

    void GetRandomProblem(xyLoc &l1, xyLoc &l2)
    {
        if (algorithms_.size() > 0)
            algorithms_[0]->GetRandomProblem(l1, l2);
    }

private:
    std::vector<AnyAngleAlgorithm*> algorithms_;
    ThetaStar   *a_star_euc_, *a_star_oct_, *theta_star_, *lazy_theta_star_;
    BlockAStar  *block_a_star_;
    FieldAStar  *field_a_star_;
    ANYA        *anya_;
    SubgoalAA   *sub_1_level_a_star_, *sub_2_level_a_star_, *sub_10000_level_a_star_,
                *sub_1_level_theta_star_, *sub_2_level_theta_star_, *sub_10000_level_theta_star_;

};

#endif
