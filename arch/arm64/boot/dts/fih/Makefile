ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)

#EAG/RHD EVB
dtbo-$(CONFIG_FIH_SDM4x9_PROJS) += sdm439-mtp-overlay_eag-evb.dtbo
dtbo-$(CONFIG_FIH_SDM4x9_PROJS) += sdm429-mtp-overlay_eag-evb.dtbo
dtbo-$(CONFIG_FIH_SDM4x9_PROJS) += sdm439-mtp-overlay_rhd-evb.dtbo
dtbo-$(CONFIG_FIH_SDM4x9_PROJS) += sdm429-mtp-overlay_rhd-evb.dtbo
#EAG/RHD/TRD preEVT/preEVT2
dtbo-$(CONFIG_FIH_SDM4x9_PROJS) += sdm439-mtp-overlay_eag-pre-evt.dtbo
dtbo-$(CONFIG_FIH_SDM4x9_PROJS) += sdm429-mtp-overlay_eag-pre-evt.dtbo
dtbo-$(CONFIG_FIH_SDM4x9_PROJS) += sdm439-mtp-overlay_rhd-pre-evt.dtbo
dtbo-$(CONFIG_FIH_SDM4x9_PROJS) += sdm429-mtp-overlay_rhd-pre-evt.dtbo

#EAG/RHD/TRD EVT1
dtbo-$(CONFIG_FIH_SDM4x9_PROJS) += sdm439-mtp-overlay_eag-evt.dtbo
dtbo-$(CONFIG_FIH_SDM4x9_PROJS) += sdm429-mtp-overlay_eag-evt.dtbo
dtbo-$(CONFIG_FIH_SDM4x9_PROJS) += sdm439-mtp-overlay_rhd-evt.dtbo
dtbo-$(CONFIG_FIH_SDM4x9_PROJS) += sdm429-mtp-overlay_rhd-evt.dtbo
  
sdm439-mtp-overlay_eag-evb.dtbo-base := sdm439-mtp-fih.dtb
sdm429-mtp-overlay_eag-evb.dtbo-base := sdm429-mtp-fih.dtb
sdm439-mtp-overlay_rhd-evb.dtbo-base := sdm439-mtp-fih.dtb
sdm429-mtp-overlay_rhd-evb.dtbo-base := sdm429-mtp-fih.dtb

sdm439-mtp-overlay_eag-pre-evt.dtbo-base := sdm439-mtp-fih.dtb
sdm429-mtp-overlay_eag-pre-evt.dtbo-base := sdm429-mtp-fih.dtb
sdm439-mtp-overlay_rhd-pre-evt.dtbo-base := sdm439-mtp-fih.dtb
sdm429-mtp-overlay_rhd-pre-evt.dtbo-base := sdm429-mtp-fih.dtb

sdm439-mtp-overlay_eag-evt.dtbo-base := sdm439-mtp-fih.dtb
sdm429-mtp-overlay_eag-evt.dtbo-base := sdm429-mtp-fih.dtb
sdm439-mtp-overlay_rhd-evt.dtbo-base := sdm439-mtp-fih.dtb
sdm429-mtp-overlay_rhd-evt.dtbo-base := sdm429-mtp-fih.dtb

endif

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb
