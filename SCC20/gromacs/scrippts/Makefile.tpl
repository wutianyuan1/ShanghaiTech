GMX ?= ${HOME}/gromacs-2020.4/build-icc18/bin/gmx
PROTEIN ?= 7C22
BOX_TYPE ?= cubic
BOX_DISTANCE ?= 0.13
SOLVATE ?= spc216.gro
PME_FLAGS ?= -pme gpu -npme 1
BONDED_FLAGS ?= -bonded gpu
NB_FLAGS ?= -nb gpu
MDRUN_FLAGS ?= -pin on -ntmpi 4 -ntomp 4 ${NB_FLAGS} -v

FILE_NAME ?= ${PROTEIN}_${FORCE_FIELD}

all: md

${PROTEIN}_newbox.gro:
	${GMX} editconf -f ${FILE_NAME}.gro -o $@ -c -d ${BOX_DISTANCE} -bt ${BOX_TYPE}

${PROTEIN}_solv.gro: ${PROTEIN}_newbox.gro
	${GMX} solvate -cp $< -cs ${SOLVATE} -o $@ -p ${FILE_NAME}.top

em.tpr: ${PROTEIN}_solv.gro
	${GMX} grompp -f em.mdp -c $< -r $< -p ${FILE_NAME}.top -o $@

em.gro em.edr em.trr em: em.tpr
	${GMX} mdrun ${MDRUN_FLAGS} -deffnm em

pr.tpr: em.gro
	${GMX} grompp -f pr.mdp -c $< -r $< -p ${FILE_NAME}.top -o $@

pr.gro pr.edr pr.trr pr.xtc pr: pr.tpr
	${GMX} mdrun ${MDRUN_FLAGS} -deffnm em

md.tpr: pr.gro
	${GMX} grompp -f md.mdp -c $< -r $< -p ${FILE_NAME}.top -o $@

md.gro md.edr md.xtc md.cpt md: md.tpr
	${GMX} mdrun ${MDRUN_FLAGS} ${BONDED_FLAGS} ${PME_FLAGS} -deffnm md

clean_preprocess:
	rm -rf ${PROTEIN}_processed.gro ${PROTEIN}_newbox.gro ${PROTEIN}_solv.gro

clean_em:
	rm -rf em.tpr em.gro em.edr em.trr em.log

clean_pr:
	rm -rf pr.tpr pr.gro pr.edr pr.trr pr.log pr.xtc

clean_md:
	rm -rf md_0_1.tpr md_0_1.gro md_0_1.edr md_0_1.xtc md_0_1.cpt md_0_1.log

clean_mdrun: clean_em clean_pr clean_md

clean_run: clean_preprocess clean_mdrun

clean_backup:
	rm -rf \#*

clean: clean_preprocess clean_run clean_backup

