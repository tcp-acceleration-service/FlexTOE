/* SPDX-License-Identifier: BSD 3-Clause License */
/* Copyright (c) 2022, University of Washington, Max Planck Institute for Software Systems, and The University of Texas at Austin */

#ifndef QMAN_H_
#define QMAN_H_

#include <nfp.h>
#include <nfp/mem_ring.h>

__export __shared __imem uint32_t qm_credits;

struct queue_bump_t
{
  union
  {
    __packed struct
    {
      uint32_t bump;
      uint32_t flow_id:16;
      uint32_t rsvd:16;
    };

    uint32_t __raw[2];
  };
};

/* QM Parameters */
#define QM_SLOT_RESOLUTION_TS_CYC_CNT   256
#define QM_SLOT_RESOLUTION_TS_CYC_SHIFT 8        // 256 = (1 << 8)
#define QM_SLOT_RESOLUTION_TS_CLK_CNT   (QM_SLOT_RESOLUTION_TS_CYC_CNT/16) // TCLK FREQ = CYC/16

/** Scheduler Queues **/
/* Each entry is 4 bytes */
#define QM_NUM_SLOTS            512
#define QM_NUM_SLOTS_PER_CYCLE  (QM_NUM_SLOTS/2)
#define QM_FIFO_LEN     16384
#define QM_SLOT_LEN     512
#define QM_FIFO_RNUM    511
#define QM_SLOT_RNUM_BASE  (QM_FIFO_RNUM + 1)
#define QM_SCHED_RNUM_BASE 256
#define QM_SCHED_RING_SIZE 8192
MEM_RING_INIT(qm_bump_ring, 4096);
MEM_RING_INIT_RN(qm_sched_ring0, QM_SCHED_RING_SIZE, 256);
MEM_RING_INIT_RN(qm_sched_ring1, QM_SCHED_RING_SIZE, 257);
MEM_RING_INIT_RN(qm_sched_ring2, QM_SCHED_RING_SIZE, 258);
MEM_RING_INIT_RN(qm_sched_ring3, QM_SCHED_RING_SIZE, 259);
MEM_RING_INIT_RN(qm_slot_fifo, 65536, 511);
MEM_RING_INIT_RN(qm_slot_0, 2048, 512);
MEM_RING_INIT_RN(qm_slot_1, 2048, 513);
MEM_RING_INIT_RN(qm_slot_2, 2048, 514);
MEM_RING_INIT_RN(qm_slot_3, 2048, 515);
MEM_RING_INIT_RN(qm_slot_4, 2048, 516);
MEM_RING_INIT_RN(qm_slot_5, 2048, 517);
MEM_RING_INIT_RN(qm_slot_6, 2048, 518);
MEM_RING_INIT_RN(qm_slot_7, 2048, 519);
MEM_RING_INIT_RN(qm_slot_8, 2048, 520);
MEM_RING_INIT_RN(qm_slot_9, 2048, 521);
MEM_RING_INIT_RN(qm_slot_10, 2048, 522);
MEM_RING_INIT_RN(qm_slot_11, 2048, 523);
MEM_RING_INIT_RN(qm_slot_12, 2048, 524);
MEM_RING_INIT_RN(qm_slot_13, 2048, 525);
MEM_RING_INIT_RN(qm_slot_14, 2048, 526);
MEM_RING_INIT_RN(qm_slot_15, 2048, 527);
MEM_RING_INIT_RN(qm_slot_16, 2048, 528);
MEM_RING_INIT_RN(qm_slot_17, 2048, 529);
MEM_RING_INIT_RN(qm_slot_18, 2048, 530);
MEM_RING_INIT_RN(qm_slot_19, 2048, 531);
MEM_RING_INIT_RN(qm_slot_20, 2048, 532);
MEM_RING_INIT_RN(qm_slot_21, 2048, 533);
MEM_RING_INIT_RN(qm_slot_22, 2048, 534);
MEM_RING_INIT_RN(qm_slot_23, 2048, 535);
MEM_RING_INIT_RN(qm_slot_24, 2048, 536);
MEM_RING_INIT_RN(qm_slot_25, 2048, 537);
MEM_RING_INIT_RN(qm_slot_26, 2048, 538);
MEM_RING_INIT_RN(qm_slot_27, 2048, 539);
MEM_RING_INIT_RN(qm_slot_28, 2048, 540);
MEM_RING_INIT_RN(qm_slot_29, 2048, 541);
MEM_RING_INIT_RN(qm_slot_30, 2048, 542);
MEM_RING_INIT_RN(qm_slot_31, 2048, 543);
MEM_RING_INIT_RN(qm_slot_32, 2048, 544);
MEM_RING_INIT_RN(qm_slot_33, 2048, 545);
MEM_RING_INIT_RN(qm_slot_34, 2048, 546);
MEM_RING_INIT_RN(qm_slot_35, 2048, 547);
MEM_RING_INIT_RN(qm_slot_36, 2048, 548);
MEM_RING_INIT_RN(qm_slot_37, 2048, 549);
MEM_RING_INIT_RN(qm_slot_38, 2048, 550);
MEM_RING_INIT_RN(qm_slot_39, 2048, 551);
MEM_RING_INIT_RN(qm_slot_40, 2048, 552);
MEM_RING_INIT_RN(qm_slot_41, 2048, 553);
MEM_RING_INIT_RN(qm_slot_42, 2048, 554);
MEM_RING_INIT_RN(qm_slot_43, 2048, 555);
MEM_RING_INIT_RN(qm_slot_44, 2048, 556);
MEM_RING_INIT_RN(qm_slot_45, 2048, 557);
MEM_RING_INIT_RN(qm_slot_46, 2048, 558);
MEM_RING_INIT_RN(qm_slot_47, 2048, 559);
MEM_RING_INIT_RN(qm_slot_48, 2048, 560);
MEM_RING_INIT_RN(qm_slot_49, 2048, 561);
MEM_RING_INIT_RN(qm_slot_50, 2048, 562);
MEM_RING_INIT_RN(qm_slot_51, 2048, 563);
MEM_RING_INIT_RN(qm_slot_52, 2048, 564);
MEM_RING_INIT_RN(qm_slot_53, 2048, 565);
MEM_RING_INIT_RN(qm_slot_54, 2048, 566);
MEM_RING_INIT_RN(qm_slot_55, 2048, 567);
MEM_RING_INIT_RN(qm_slot_56, 2048, 568);
MEM_RING_INIT_RN(qm_slot_57, 2048, 569);
MEM_RING_INIT_RN(qm_slot_58, 2048, 570);
MEM_RING_INIT_RN(qm_slot_59, 2048, 571);
MEM_RING_INIT_RN(qm_slot_60, 2048, 572);
MEM_RING_INIT_RN(qm_slot_61, 2048, 573);
MEM_RING_INIT_RN(qm_slot_62, 2048, 574);
MEM_RING_INIT_RN(qm_slot_63, 2048, 575);
MEM_RING_INIT_RN(qm_slot_64, 2048, 576);
MEM_RING_INIT_RN(qm_slot_65, 2048, 577);
MEM_RING_INIT_RN(qm_slot_66, 2048, 578);
MEM_RING_INIT_RN(qm_slot_67, 2048, 579);
MEM_RING_INIT_RN(qm_slot_68, 2048, 580);
MEM_RING_INIT_RN(qm_slot_69, 2048, 581);
MEM_RING_INIT_RN(qm_slot_70, 2048, 582);
MEM_RING_INIT_RN(qm_slot_71, 2048, 583);
MEM_RING_INIT_RN(qm_slot_72, 2048, 584);
MEM_RING_INIT_RN(qm_slot_73, 2048, 585);
MEM_RING_INIT_RN(qm_slot_74, 2048, 586);
MEM_RING_INIT_RN(qm_slot_75, 2048, 587);
MEM_RING_INIT_RN(qm_slot_76, 2048, 588);
MEM_RING_INIT_RN(qm_slot_77, 2048, 589);
MEM_RING_INIT_RN(qm_slot_78, 2048, 590);
MEM_RING_INIT_RN(qm_slot_79, 2048, 591);
MEM_RING_INIT_RN(qm_slot_80, 2048, 592);
MEM_RING_INIT_RN(qm_slot_81, 2048, 593);
MEM_RING_INIT_RN(qm_slot_82, 2048, 594);
MEM_RING_INIT_RN(qm_slot_83, 2048, 595);
MEM_RING_INIT_RN(qm_slot_84, 2048, 596);
MEM_RING_INIT_RN(qm_slot_85, 2048, 597);
MEM_RING_INIT_RN(qm_slot_86, 2048, 598);
MEM_RING_INIT_RN(qm_slot_87, 2048, 599);
MEM_RING_INIT_RN(qm_slot_88, 2048, 600);
MEM_RING_INIT_RN(qm_slot_89, 2048, 601);
MEM_RING_INIT_RN(qm_slot_90, 2048, 602);
MEM_RING_INIT_RN(qm_slot_91, 2048, 603);
MEM_RING_INIT_RN(qm_slot_92, 2048, 604);
MEM_RING_INIT_RN(qm_slot_93, 2048, 605);
MEM_RING_INIT_RN(qm_slot_94, 2048, 606);
MEM_RING_INIT_RN(qm_slot_95, 2048, 607);
MEM_RING_INIT_RN(qm_slot_96, 2048, 608);
MEM_RING_INIT_RN(qm_slot_97, 2048, 609);
MEM_RING_INIT_RN(qm_slot_98, 2048, 610);
MEM_RING_INIT_RN(qm_slot_99, 2048, 611);
MEM_RING_INIT_RN(qm_slot_100, 2048, 612);
MEM_RING_INIT_RN(qm_slot_101, 2048, 613);
MEM_RING_INIT_RN(qm_slot_102, 2048, 614);
MEM_RING_INIT_RN(qm_slot_103, 2048, 615);
MEM_RING_INIT_RN(qm_slot_104, 2048, 616);
MEM_RING_INIT_RN(qm_slot_105, 2048, 617);
MEM_RING_INIT_RN(qm_slot_106, 2048, 618);
MEM_RING_INIT_RN(qm_slot_107, 2048, 619);
MEM_RING_INIT_RN(qm_slot_108, 2048, 620);
MEM_RING_INIT_RN(qm_slot_109, 2048, 621);
MEM_RING_INIT_RN(qm_slot_110, 2048, 622);
MEM_RING_INIT_RN(qm_slot_111, 2048, 623);
MEM_RING_INIT_RN(qm_slot_112, 2048, 624);
MEM_RING_INIT_RN(qm_slot_113, 2048, 625);
MEM_RING_INIT_RN(qm_slot_114, 2048, 626);
MEM_RING_INIT_RN(qm_slot_115, 2048, 627);
MEM_RING_INIT_RN(qm_slot_116, 2048, 628);
MEM_RING_INIT_RN(qm_slot_117, 2048, 629);
MEM_RING_INIT_RN(qm_slot_118, 2048, 630);
MEM_RING_INIT_RN(qm_slot_119, 2048, 631);
MEM_RING_INIT_RN(qm_slot_120, 2048, 632);
MEM_RING_INIT_RN(qm_slot_121, 2048, 633);
MEM_RING_INIT_RN(qm_slot_122, 2048, 634);
MEM_RING_INIT_RN(qm_slot_123, 2048, 635);
MEM_RING_INIT_RN(qm_slot_124, 2048, 636);
MEM_RING_INIT_RN(qm_slot_125, 2048, 637);
MEM_RING_INIT_RN(qm_slot_126, 2048, 638);
MEM_RING_INIT_RN(qm_slot_127, 2048, 639);
MEM_RING_INIT_RN(qm_slot_128, 2048, 640);
MEM_RING_INIT_RN(qm_slot_129, 2048, 641);
MEM_RING_INIT_RN(qm_slot_130, 2048, 642);
MEM_RING_INIT_RN(qm_slot_131, 2048, 643);
MEM_RING_INIT_RN(qm_slot_132, 2048, 644);
MEM_RING_INIT_RN(qm_slot_133, 2048, 645);
MEM_RING_INIT_RN(qm_slot_134, 2048, 646);
MEM_RING_INIT_RN(qm_slot_135, 2048, 647);
MEM_RING_INIT_RN(qm_slot_136, 2048, 648);
MEM_RING_INIT_RN(qm_slot_137, 2048, 649);
MEM_RING_INIT_RN(qm_slot_138, 2048, 650);
MEM_RING_INIT_RN(qm_slot_139, 2048, 651);
MEM_RING_INIT_RN(qm_slot_140, 2048, 652);
MEM_RING_INIT_RN(qm_slot_141, 2048, 653);
MEM_RING_INIT_RN(qm_slot_142, 2048, 654);
MEM_RING_INIT_RN(qm_slot_143, 2048, 655);
MEM_RING_INIT_RN(qm_slot_144, 2048, 656);
MEM_RING_INIT_RN(qm_slot_145, 2048, 657);
MEM_RING_INIT_RN(qm_slot_146, 2048, 658);
MEM_RING_INIT_RN(qm_slot_147, 2048, 659);
MEM_RING_INIT_RN(qm_slot_148, 2048, 660);
MEM_RING_INIT_RN(qm_slot_149, 2048, 661);
MEM_RING_INIT_RN(qm_slot_150, 2048, 662);
MEM_RING_INIT_RN(qm_slot_151, 2048, 663);
MEM_RING_INIT_RN(qm_slot_152, 2048, 664);
MEM_RING_INIT_RN(qm_slot_153, 2048, 665);
MEM_RING_INIT_RN(qm_slot_154, 2048, 666);
MEM_RING_INIT_RN(qm_slot_155, 2048, 667);
MEM_RING_INIT_RN(qm_slot_156, 2048, 668);
MEM_RING_INIT_RN(qm_slot_157, 2048, 669);
MEM_RING_INIT_RN(qm_slot_158, 2048, 670);
MEM_RING_INIT_RN(qm_slot_159, 2048, 671);
MEM_RING_INIT_RN(qm_slot_160, 2048, 672);
MEM_RING_INIT_RN(qm_slot_161, 2048, 673);
MEM_RING_INIT_RN(qm_slot_162, 2048, 674);
MEM_RING_INIT_RN(qm_slot_163, 2048, 675);
MEM_RING_INIT_RN(qm_slot_164, 2048, 676);
MEM_RING_INIT_RN(qm_slot_165, 2048, 677);
MEM_RING_INIT_RN(qm_slot_166, 2048, 678);
MEM_RING_INIT_RN(qm_slot_167, 2048, 679);
MEM_RING_INIT_RN(qm_slot_168, 2048, 680);
MEM_RING_INIT_RN(qm_slot_169, 2048, 681);
MEM_RING_INIT_RN(qm_slot_170, 2048, 682);
MEM_RING_INIT_RN(qm_slot_171, 2048, 683);
MEM_RING_INIT_RN(qm_slot_172, 2048, 684);
MEM_RING_INIT_RN(qm_slot_173, 2048, 685);
MEM_RING_INIT_RN(qm_slot_174, 2048, 686);
MEM_RING_INIT_RN(qm_slot_175, 2048, 687);
MEM_RING_INIT_RN(qm_slot_176, 2048, 688);
MEM_RING_INIT_RN(qm_slot_177, 2048, 689);
MEM_RING_INIT_RN(qm_slot_178, 2048, 690);
MEM_RING_INIT_RN(qm_slot_179, 2048, 691);
MEM_RING_INIT_RN(qm_slot_180, 2048, 692);
MEM_RING_INIT_RN(qm_slot_181, 2048, 693);
MEM_RING_INIT_RN(qm_slot_182, 2048, 694);
MEM_RING_INIT_RN(qm_slot_183, 2048, 695);
MEM_RING_INIT_RN(qm_slot_184, 2048, 696);
MEM_RING_INIT_RN(qm_slot_185, 2048, 697);
MEM_RING_INIT_RN(qm_slot_186, 2048, 698);
MEM_RING_INIT_RN(qm_slot_187, 2048, 699);
MEM_RING_INIT_RN(qm_slot_188, 2048, 700);
MEM_RING_INIT_RN(qm_slot_189, 2048, 701);
MEM_RING_INIT_RN(qm_slot_190, 2048, 702);
MEM_RING_INIT_RN(qm_slot_191, 2048, 703);
MEM_RING_INIT_RN(qm_slot_192, 2048, 704);
MEM_RING_INIT_RN(qm_slot_193, 2048, 705);
MEM_RING_INIT_RN(qm_slot_194, 2048, 706);
MEM_RING_INIT_RN(qm_slot_195, 2048, 707);
MEM_RING_INIT_RN(qm_slot_196, 2048, 708);
MEM_RING_INIT_RN(qm_slot_197, 2048, 709);
MEM_RING_INIT_RN(qm_slot_198, 2048, 710);
MEM_RING_INIT_RN(qm_slot_199, 2048, 711);
MEM_RING_INIT_RN(qm_slot_200, 2048, 712);
MEM_RING_INIT_RN(qm_slot_201, 2048, 713);
MEM_RING_INIT_RN(qm_slot_202, 2048, 714);
MEM_RING_INIT_RN(qm_slot_203, 2048, 715);
MEM_RING_INIT_RN(qm_slot_204, 2048, 716);
MEM_RING_INIT_RN(qm_slot_205, 2048, 717);
MEM_RING_INIT_RN(qm_slot_206, 2048, 718);
MEM_RING_INIT_RN(qm_slot_207, 2048, 719);
MEM_RING_INIT_RN(qm_slot_208, 2048, 720);
MEM_RING_INIT_RN(qm_slot_209, 2048, 721);
MEM_RING_INIT_RN(qm_slot_210, 2048, 722);
MEM_RING_INIT_RN(qm_slot_211, 2048, 723);
MEM_RING_INIT_RN(qm_slot_212, 2048, 724);
MEM_RING_INIT_RN(qm_slot_213, 2048, 725);
MEM_RING_INIT_RN(qm_slot_214, 2048, 726);
MEM_RING_INIT_RN(qm_slot_215, 2048, 727);
MEM_RING_INIT_RN(qm_slot_216, 2048, 728);
MEM_RING_INIT_RN(qm_slot_217, 2048, 729);
MEM_RING_INIT_RN(qm_slot_218, 2048, 730);
MEM_RING_INIT_RN(qm_slot_219, 2048, 731);
MEM_RING_INIT_RN(qm_slot_220, 2048, 732);
MEM_RING_INIT_RN(qm_slot_221, 2048, 733);
MEM_RING_INIT_RN(qm_slot_222, 2048, 734);
MEM_RING_INIT_RN(qm_slot_223, 2048, 735);
MEM_RING_INIT_RN(qm_slot_224, 2048, 736);
MEM_RING_INIT_RN(qm_slot_225, 2048, 737);
MEM_RING_INIT_RN(qm_slot_226, 2048, 738);
MEM_RING_INIT_RN(qm_slot_227, 2048, 739);
MEM_RING_INIT_RN(qm_slot_228, 2048, 740);
MEM_RING_INIT_RN(qm_slot_229, 2048, 741);
MEM_RING_INIT_RN(qm_slot_230, 2048, 742);
MEM_RING_INIT_RN(qm_slot_231, 2048, 743);
MEM_RING_INIT_RN(qm_slot_232, 2048, 744);
MEM_RING_INIT_RN(qm_slot_233, 2048, 745);
MEM_RING_INIT_RN(qm_slot_234, 2048, 746);
MEM_RING_INIT_RN(qm_slot_235, 2048, 747);
MEM_RING_INIT_RN(qm_slot_236, 2048, 748);
MEM_RING_INIT_RN(qm_slot_237, 2048, 749);
MEM_RING_INIT_RN(qm_slot_238, 2048, 750);
MEM_RING_INIT_RN(qm_slot_239, 2048, 751);
MEM_RING_INIT_RN(qm_slot_240, 2048, 752);
MEM_RING_INIT_RN(qm_slot_241, 2048, 753);
MEM_RING_INIT_RN(qm_slot_242, 2048, 754);
MEM_RING_INIT_RN(qm_slot_243, 2048, 755);
MEM_RING_INIT_RN(qm_slot_244, 2048, 756);
MEM_RING_INIT_RN(qm_slot_245, 2048, 757);
MEM_RING_INIT_RN(qm_slot_246, 2048, 758);
MEM_RING_INIT_RN(qm_slot_247, 2048, 759);
MEM_RING_INIT_RN(qm_slot_248, 2048, 760);
MEM_RING_INIT_RN(qm_slot_249, 2048, 761);
MEM_RING_INIT_RN(qm_slot_250, 2048, 762);
MEM_RING_INIT_RN(qm_slot_251, 2048, 763);
MEM_RING_INIT_RN(qm_slot_252, 2048, 764);
MEM_RING_INIT_RN(qm_slot_253, 2048, 765);
MEM_RING_INIT_RN(qm_slot_254, 2048, 766);
MEM_RING_INIT_RN(qm_slot_255, 2048, 767);
MEM_RING_INIT_RN(qm_slot_256, 2048, 768);
MEM_RING_INIT_RN(qm_slot_257, 2048, 769);
MEM_RING_INIT_RN(qm_slot_258, 2048, 770);
MEM_RING_INIT_RN(qm_slot_259, 2048, 771);
MEM_RING_INIT_RN(qm_slot_260, 2048, 772);
MEM_RING_INIT_RN(qm_slot_261, 2048, 773);
MEM_RING_INIT_RN(qm_slot_262, 2048, 774);
MEM_RING_INIT_RN(qm_slot_263, 2048, 775);
MEM_RING_INIT_RN(qm_slot_264, 2048, 776);
MEM_RING_INIT_RN(qm_slot_265, 2048, 777);
MEM_RING_INIT_RN(qm_slot_266, 2048, 778);
MEM_RING_INIT_RN(qm_slot_267, 2048, 779);
MEM_RING_INIT_RN(qm_slot_268, 2048, 780);
MEM_RING_INIT_RN(qm_slot_269, 2048, 781);
MEM_RING_INIT_RN(qm_slot_270, 2048, 782);
MEM_RING_INIT_RN(qm_slot_271, 2048, 783);
MEM_RING_INIT_RN(qm_slot_272, 2048, 784);
MEM_RING_INIT_RN(qm_slot_273, 2048, 785);
MEM_RING_INIT_RN(qm_slot_274, 2048, 786);
MEM_RING_INIT_RN(qm_slot_275, 2048, 787);
MEM_RING_INIT_RN(qm_slot_276, 2048, 788);
MEM_RING_INIT_RN(qm_slot_277, 2048, 789);
MEM_RING_INIT_RN(qm_slot_278, 2048, 790);
MEM_RING_INIT_RN(qm_slot_279, 2048, 791);
MEM_RING_INIT_RN(qm_slot_280, 2048, 792);
MEM_RING_INIT_RN(qm_slot_281, 2048, 793);
MEM_RING_INIT_RN(qm_slot_282, 2048, 794);
MEM_RING_INIT_RN(qm_slot_283, 2048, 795);
MEM_RING_INIT_RN(qm_slot_284, 2048, 796);
MEM_RING_INIT_RN(qm_slot_285, 2048, 797);
MEM_RING_INIT_RN(qm_slot_286, 2048, 798);
MEM_RING_INIT_RN(qm_slot_287, 2048, 799);
MEM_RING_INIT_RN(qm_slot_288, 2048, 800);
MEM_RING_INIT_RN(qm_slot_289, 2048, 801);
MEM_RING_INIT_RN(qm_slot_290, 2048, 802);
MEM_RING_INIT_RN(qm_slot_291, 2048, 803);
MEM_RING_INIT_RN(qm_slot_292, 2048, 804);
MEM_RING_INIT_RN(qm_slot_293, 2048, 805);
MEM_RING_INIT_RN(qm_slot_294, 2048, 806);
MEM_RING_INIT_RN(qm_slot_295, 2048, 807);
MEM_RING_INIT_RN(qm_slot_296, 2048, 808);
MEM_RING_INIT_RN(qm_slot_297, 2048, 809);
MEM_RING_INIT_RN(qm_slot_298, 2048, 810);
MEM_RING_INIT_RN(qm_slot_299, 2048, 811);
MEM_RING_INIT_RN(qm_slot_300, 2048, 812);
MEM_RING_INIT_RN(qm_slot_301, 2048, 813);
MEM_RING_INIT_RN(qm_slot_302, 2048, 814);
MEM_RING_INIT_RN(qm_slot_303, 2048, 815);
MEM_RING_INIT_RN(qm_slot_304, 2048, 816);
MEM_RING_INIT_RN(qm_slot_305, 2048, 817);
MEM_RING_INIT_RN(qm_slot_306, 2048, 818);
MEM_RING_INIT_RN(qm_slot_307, 2048, 819);
MEM_RING_INIT_RN(qm_slot_308, 2048, 820);
MEM_RING_INIT_RN(qm_slot_309, 2048, 821);
MEM_RING_INIT_RN(qm_slot_310, 2048, 822);
MEM_RING_INIT_RN(qm_slot_311, 2048, 823);
MEM_RING_INIT_RN(qm_slot_312, 2048, 824);
MEM_RING_INIT_RN(qm_slot_313, 2048, 825);
MEM_RING_INIT_RN(qm_slot_314, 2048, 826);
MEM_RING_INIT_RN(qm_slot_315, 2048, 827);
MEM_RING_INIT_RN(qm_slot_316, 2048, 828);
MEM_RING_INIT_RN(qm_slot_317, 2048, 829);
MEM_RING_INIT_RN(qm_slot_318, 2048, 830);
MEM_RING_INIT_RN(qm_slot_319, 2048, 831);
MEM_RING_INIT_RN(qm_slot_320, 2048, 832);
MEM_RING_INIT_RN(qm_slot_321, 2048, 833);
MEM_RING_INIT_RN(qm_slot_322, 2048, 834);
MEM_RING_INIT_RN(qm_slot_323, 2048, 835);
MEM_RING_INIT_RN(qm_slot_324, 2048, 836);
MEM_RING_INIT_RN(qm_slot_325, 2048, 837);
MEM_RING_INIT_RN(qm_slot_326, 2048, 838);
MEM_RING_INIT_RN(qm_slot_327, 2048, 839);
MEM_RING_INIT_RN(qm_slot_328, 2048, 840);
MEM_RING_INIT_RN(qm_slot_329, 2048, 841);
MEM_RING_INIT_RN(qm_slot_330, 2048, 842);
MEM_RING_INIT_RN(qm_slot_331, 2048, 843);
MEM_RING_INIT_RN(qm_slot_332, 2048, 844);
MEM_RING_INIT_RN(qm_slot_333, 2048, 845);
MEM_RING_INIT_RN(qm_slot_334, 2048, 846);
MEM_RING_INIT_RN(qm_slot_335, 2048, 847);
MEM_RING_INIT_RN(qm_slot_336, 2048, 848);
MEM_RING_INIT_RN(qm_slot_337, 2048, 849);
MEM_RING_INIT_RN(qm_slot_338, 2048, 850);
MEM_RING_INIT_RN(qm_slot_339, 2048, 851);
MEM_RING_INIT_RN(qm_slot_340, 2048, 852);
MEM_RING_INIT_RN(qm_slot_341, 2048, 853);
MEM_RING_INIT_RN(qm_slot_342, 2048, 854);
MEM_RING_INIT_RN(qm_slot_343, 2048, 855);
MEM_RING_INIT_RN(qm_slot_344, 2048, 856);
MEM_RING_INIT_RN(qm_slot_345, 2048, 857);
MEM_RING_INIT_RN(qm_slot_346, 2048, 858);
MEM_RING_INIT_RN(qm_slot_347, 2048, 859);
MEM_RING_INIT_RN(qm_slot_348, 2048, 860);
MEM_RING_INIT_RN(qm_slot_349, 2048, 861);
MEM_RING_INIT_RN(qm_slot_350, 2048, 862);
MEM_RING_INIT_RN(qm_slot_351, 2048, 863);
MEM_RING_INIT_RN(qm_slot_352, 2048, 864);
MEM_RING_INIT_RN(qm_slot_353, 2048, 865);
MEM_RING_INIT_RN(qm_slot_354, 2048, 866);
MEM_RING_INIT_RN(qm_slot_355, 2048, 867);
MEM_RING_INIT_RN(qm_slot_356, 2048, 868);
MEM_RING_INIT_RN(qm_slot_357, 2048, 869);
MEM_RING_INIT_RN(qm_slot_358, 2048, 870);
MEM_RING_INIT_RN(qm_slot_359, 2048, 871);
MEM_RING_INIT_RN(qm_slot_360, 2048, 872);
MEM_RING_INIT_RN(qm_slot_361, 2048, 873);
MEM_RING_INIT_RN(qm_slot_362, 2048, 874);
MEM_RING_INIT_RN(qm_slot_363, 2048, 875);
MEM_RING_INIT_RN(qm_slot_364, 2048, 876);
MEM_RING_INIT_RN(qm_slot_365, 2048, 877);
MEM_RING_INIT_RN(qm_slot_366, 2048, 878);
MEM_RING_INIT_RN(qm_slot_367, 2048, 879);
MEM_RING_INIT_RN(qm_slot_368, 2048, 880);
MEM_RING_INIT_RN(qm_slot_369, 2048, 881);
MEM_RING_INIT_RN(qm_slot_370, 2048, 882);
MEM_RING_INIT_RN(qm_slot_371, 2048, 883);
MEM_RING_INIT_RN(qm_slot_372, 2048, 884);
MEM_RING_INIT_RN(qm_slot_373, 2048, 885);
MEM_RING_INIT_RN(qm_slot_374, 2048, 886);
MEM_RING_INIT_RN(qm_slot_375, 2048, 887);
MEM_RING_INIT_RN(qm_slot_376, 2048, 888);
MEM_RING_INIT_RN(qm_slot_377, 2048, 889);
MEM_RING_INIT_RN(qm_slot_378, 2048, 890);
MEM_RING_INIT_RN(qm_slot_379, 2048, 891);
MEM_RING_INIT_RN(qm_slot_380, 2048, 892);
MEM_RING_INIT_RN(qm_slot_381, 2048, 893);
MEM_RING_INIT_RN(qm_slot_382, 2048, 894);
MEM_RING_INIT_RN(qm_slot_383, 2048, 895);
MEM_RING_INIT_RN(qm_slot_384, 2048, 896);
MEM_RING_INIT_RN(qm_slot_385, 2048, 897);
MEM_RING_INIT_RN(qm_slot_386, 2048, 898);
MEM_RING_INIT_RN(qm_slot_387, 2048, 899);
MEM_RING_INIT_RN(qm_slot_388, 2048, 900);
MEM_RING_INIT_RN(qm_slot_389, 2048, 901);
MEM_RING_INIT_RN(qm_slot_390, 2048, 902);
MEM_RING_INIT_RN(qm_slot_391, 2048, 903);
MEM_RING_INIT_RN(qm_slot_392, 2048, 904);
MEM_RING_INIT_RN(qm_slot_393, 2048, 905);
MEM_RING_INIT_RN(qm_slot_394, 2048, 906);
MEM_RING_INIT_RN(qm_slot_395, 2048, 907);
MEM_RING_INIT_RN(qm_slot_396, 2048, 908);
MEM_RING_INIT_RN(qm_slot_397, 2048, 909);
MEM_RING_INIT_RN(qm_slot_398, 2048, 910);
MEM_RING_INIT_RN(qm_slot_399, 2048, 911);
MEM_RING_INIT_RN(qm_slot_400, 2048, 912);
MEM_RING_INIT_RN(qm_slot_401, 2048, 913);
MEM_RING_INIT_RN(qm_slot_402, 2048, 914);
MEM_RING_INIT_RN(qm_slot_403, 2048, 915);
MEM_RING_INIT_RN(qm_slot_404, 2048, 916);
MEM_RING_INIT_RN(qm_slot_405, 2048, 917);
MEM_RING_INIT_RN(qm_slot_406, 2048, 918);
MEM_RING_INIT_RN(qm_slot_407, 2048, 919);
MEM_RING_INIT_RN(qm_slot_408, 2048, 920);
MEM_RING_INIT_RN(qm_slot_409, 2048, 921);
MEM_RING_INIT_RN(qm_slot_410, 2048, 922);
MEM_RING_INIT_RN(qm_slot_411, 2048, 923);
MEM_RING_INIT_RN(qm_slot_412, 2048, 924);
MEM_RING_INIT_RN(qm_slot_413, 2048, 925);
MEM_RING_INIT_RN(qm_slot_414, 2048, 926);
MEM_RING_INIT_RN(qm_slot_415, 2048, 927);
MEM_RING_INIT_RN(qm_slot_416, 2048, 928);
MEM_RING_INIT_RN(qm_slot_417, 2048, 929);
MEM_RING_INIT_RN(qm_slot_418, 2048, 930);
MEM_RING_INIT_RN(qm_slot_419, 2048, 931);
MEM_RING_INIT_RN(qm_slot_420, 2048, 932);
MEM_RING_INIT_RN(qm_slot_421, 2048, 933);
MEM_RING_INIT_RN(qm_slot_422, 2048, 934);
MEM_RING_INIT_RN(qm_slot_423, 2048, 935);
MEM_RING_INIT_RN(qm_slot_424, 2048, 936);
MEM_RING_INIT_RN(qm_slot_425, 2048, 937);
MEM_RING_INIT_RN(qm_slot_426, 2048, 938);
MEM_RING_INIT_RN(qm_slot_427, 2048, 939);
MEM_RING_INIT_RN(qm_slot_428, 2048, 940);
MEM_RING_INIT_RN(qm_slot_429, 2048, 941);
MEM_RING_INIT_RN(qm_slot_430, 2048, 942);
MEM_RING_INIT_RN(qm_slot_431, 2048, 943);
MEM_RING_INIT_RN(qm_slot_432, 2048, 944);
MEM_RING_INIT_RN(qm_slot_433, 2048, 945);
MEM_RING_INIT_RN(qm_slot_434, 2048, 946);
MEM_RING_INIT_RN(qm_slot_435, 2048, 947);
MEM_RING_INIT_RN(qm_slot_436, 2048, 948);
MEM_RING_INIT_RN(qm_slot_437, 2048, 949);
MEM_RING_INIT_RN(qm_slot_438, 2048, 950);
MEM_RING_INIT_RN(qm_slot_439, 2048, 951);
MEM_RING_INIT_RN(qm_slot_440, 2048, 952);
MEM_RING_INIT_RN(qm_slot_441, 2048, 953);
MEM_RING_INIT_RN(qm_slot_442, 2048, 954);
MEM_RING_INIT_RN(qm_slot_443, 2048, 955);
MEM_RING_INIT_RN(qm_slot_444, 2048, 956);
MEM_RING_INIT_RN(qm_slot_445, 2048, 957);
MEM_RING_INIT_RN(qm_slot_446, 2048, 958);
MEM_RING_INIT_RN(qm_slot_447, 2048, 959);
MEM_RING_INIT_RN(qm_slot_448, 2048, 960);
MEM_RING_INIT_RN(qm_slot_449, 2048, 961);
MEM_RING_INIT_RN(qm_slot_450, 2048, 962);
MEM_RING_INIT_RN(qm_slot_451, 2048, 963);
MEM_RING_INIT_RN(qm_slot_452, 2048, 964);
MEM_RING_INIT_RN(qm_slot_453, 2048, 965);
MEM_RING_INIT_RN(qm_slot_454, 2048, 966);
MEM_RING_INIT_RN(qm_slot_455, 2048, 967);
MEM_RING_INIT_RN(qm_slot_456, 2048, 968);
MEM_RING_INIT_RN(qm_slot_457, 2048, 969);
MEM_RING_INIT_RN(qm_slot_458, 2048, 970);
MEM_RING_INIT_RN(qm_slot_459, 2048, 971);
MEM_RING_INIT_RN(qm_slot_460, 2048, 972);
MEM_RING_INIT_RN(qm_slot_461, 2048, 973);
MEM_RING_INIT_RN(qm_slot_462, 2048, 974);
MEM_RING_INIT_RN(qm_slot_463, 2048, 975);
MEM_RING_INIT_RN(qm_slot_464, 2048, 976);
MEM_RING_INIT_RN(qm_slot_465, 2048, 977);
MEM_RING_INIT_RN(qm_slot_466, 2048, 978);
MEM_RING_INIT_RN(qm_slot_467, 2048, 979);
MEM_RING_INIT_RN(qm_slot_468, 2048, 980);
MEM_RING_INIT_RN(qm_slot_469, 2048, 981);
MEM_RING_INIT_RN(qm_slot_470, 2048, 982);
MEM_RING_INIT_RN(qm_slot_471, 2048, 983);
MEM_RING_INIT_RN(qm_slot_472, 2048, 984);
MEM_RING_INIT_RN(qm_slot_473, 2048, 985);
MEM_RING_INIT_RN(qm_slot_474, 2048, 986);
MEM_RING_INIT_RN(qm_slot_475, 2048, 987);
MEM_RING_INIT_RN(qm_slot_476, 2048, 988);
MEM_RING_INIT_RN(qm_slot_477, 2048, 989);
MEM_RING_INIT_RN(qm_slot_478, 2048, 990);
MEM_RING_INIT_RN(qm_slot_479, 2048, 991);
MEM_RING_INIT_RN(qm_slot_480, 2048, 992);
MEM_RING_INIT_RN(qm_slot_481, 2048, 993);
MEM_RING_INIT_RN(qm_slot_482, 2048, 994);
MEM_RING_INIT_RN(qm_slot_483, 2048, 995);
MEM_RING_INIT_RN(qm_slot_484, 2048, 996);
MEM_RING_INIT_RN(qm_slot_485, 2048, 997);
MEM_RING_INIT_RN(qm_slot_486, 2048, 998);
MEM_RING_INIT_RN(qm_slot_487, 2048, 999);
MEM_RING_INIT_RN(qm_slot_488, 2048, 1000);
MEM_RING_INIT_RN(qm_slot_489, 2048, 1001);
MEM_RING_INIT_RN(qm_slot_490, 2048, 1002);
MEM_RING_INIT_RN(qm_slot_491, 2048, 1003);
MEM_RING_INIT_RN(qm_slot_492, 2048, 1004);
MEM_RING_INIT_RN(qm_slot_493, 2048, 1005);
MEM_RING_INIT_RN(qm_slot_494, 2048, 1006);
MEM_RING_INIT_RN(qm_slot_495, 2048, 1007);
MEM_RING_INIT_RN(qm_slot_496, 2048, 1008);
MEM_RING_INIT_RN(qm_slot_497, 2048, 1009);
MEM_RING_INIT_RN(qm_slot_498, 2048, 1010);
MEM_RING_INIT_RN(qm_slot_499, 2048, 1011);
MEM_RING_INIT_RN(qm_slot_500, 2048, 1012);
MEM_RING_INIT_RN(qm_slot_501, 2048, 1013);
MEM_RING_INIT_RN(qm_slot_502, 2048, 1014);
MEM_RING_INIT_RN(qm_slot_503, 2048, 1015);
MEM_RING_INIT_RN(qm_slot_504, 2048, 1016);
MEM_RING_INIT_RN(qm_slot_505, 2048, 1017);
MEM_RING_INIT_RN(qm_slot_506, 2048, 1018);
MEM_RING_INIT_RN(qm_slot_507, 2048, 1019);
MEM_RING_INIT_RN(qm_slot_508, 2048, 1020);
MEM_RING_INIT_RN(qm_slot_509, 2048, 1021);
MEM_RING_INIT_RN(qm_slot_510, 2048, 1022);
MEM_RING_INIT_RN(qm_slot_511, 2048, 1023);
#endif /* QMAN_H_ */
